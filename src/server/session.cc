#include "server/session.h"

#include "core/logger.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/channel.h"
#include "server/serial-request-context.h"
#include "server/session-connection.h"
#include "session.grpc.pb.h"

namespace zen::remote::server {

Session::Session(std::unique_ptr<ILoop> loop)
    : loop_(std::move(loop)), serial_(std::make_shared<SessionSerial>())
{
  pipe_[0] = 0;
  pipe_[1] = 0;
}

Session::~Session()
{
  if (id_ != 0) {
    grpc::ClientContext context;
    auto stub = SessionService::NewStub(session_channel_->grpc_channel());
    SessionShutdownRequest request;
    request.set_id(id_);
    EmptyResponse response;

    std::mutex mutex;
    std::condition_variable cond;
    bool done = false;
    grpc::Status status;

    stub->async()->Shutdown(&context, &request, &response,
        [&mutex, &cond, &done, &status](grpc::Status s) {
          std::lock_guard<std::mutex> lock(mutex);
          status = std::move(s);
          done = true;
          cond.notify_one();
        });

    std::unique_lock<std::mutex> lock(mutex);
    cond.wait_for(
        lock, std::chrono::milliseconds(300), [&done] { return done; });
  }

  DisableChannels();

  if (pipe_[0] != 0) {
    close(pipe_[0]);
    close(pipe_[1]);
  }

  if (control_event_source_) {
    loop_->RemoveFd(control_event_source_.get());
  }
}

void
Session::StartKeepalive()
{
  std::weak_ptr<Channel> channel_weak = session_channel_;
  auto job = CreateJob([id = id_, channel_weak](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context = std::make_unique<grpc::ClientContext>();
    auto stub = SessionService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&SessionService::Stub::PrepareAsyncKeepalive>(
            std::move(stub), std::move(context),
            [channel_weak](SessionTerminateResponse* /*response*/,
                grpc::Status* /*status*/) {
              if (auto channel = channel_weak.lock()) {
                channel->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_channel_->PushJob(std::move(job));
}

bool
Session::Connect(std::shared_ptr<IPeer> peer)
{
  if (pipe2(pipe_, O_CLOEXEC | O_NONBLOCK) == -1) return false;

  host_port_ = peer->host() + ":" + std::to_string(kGrpcPort);

  auto shared_this = std::dynamic_pointer_cast<ISession>(shared_from_this());
  session_channel_ =
      std::dynamic_pointer_cast<Channel>(CreateChannel(shared_this));

  auto stub = SessionService::NewStub(session_channel_->grpc_channel());

  grpc::ClientContext context;
  NewSessionRequest request;
  NewSessionResponse response;

  auto status = stub->New(&context, request, &response);

  if (!status.ok()) {
    LOG_DEBUG("Failed to start session: %s", status.error_message().c_str());
    close(pipe_[0]);
    close(pipe_[1]);
    session_channel_.reset();
    host_port_ = "";
    return false;
  }

  id_ = response.id();
  if (peer->wired()) {
    characteristics_ |= kWired;
  }

  control_event_source_ = std::make_unique<FdSource>();

  control_event_source_->fd = pipe_[0];
  control_event_source_->mask = FdSource::kReadable;
  control_event_source_->callback = [this](int fd, uint32_t /*mask*/) {
    SessionConnection::ControlMessage message = SessionConnection::kError;
    read(fd, &message, sizeof(message));

    HandleControlEvent(message);  // this may destroy self
  };

  loop_->AddFd(control_event_source_.get());

  connected_ = true;

  StartKeepalive();

  return true;
}

void
Session::HandleControlEvent(SessionConnection::ControlMessage message)
{
  switch (message) {
    case SessionConnection::kDisconnect:  // fall through
    case SessionConnection::kError:
      if (connected_ == false) return;
      connected_ = false;
      DisableChannels();
      this->on_disconnect();  // this may destroy self
      return;

    default:
      break;
  }
}

void
Session::AddChannel(std::weak_ptr<Channel> channel)
{
  for (auto it = channels_.begin(); it != channels_.end();) {
    if ((*it).expired()) {
      it = channels_.erase(it);
    } else {
      it++;
    }
  }

  channels_.push_back(channel);
}

void
Session::DisableChannels()
{
  for (auto channel_weak : channels_) {
    if (auto channel = channel_weak.lock()) {
      channel->Disable();
    }
  }

  if (session_channel_) {
    session_channel_->Disable();
  }
}

std::shared_ptr<ISession>
CreateSession(std::unique_ptr<ILoop> loop)
{
  return std::make_shared<Session>(std::move(loop));
}

}  // namespace zen::remote::server
