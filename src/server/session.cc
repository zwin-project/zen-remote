#include "server/session.h"

#include "core/logger.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/serial-request-context.h"
#include "server/session-connection.h"
#include "session.grpc.pb.h"

namespace zen::remote::server {

Session::Session(std::unique_ptr<ILoop> loop) : loop_(std::move(loop))
{
  pipe_[0] = 0;
  pipe_[1] = 0;
}

Session::~Session()
{
  if (id_ != 0) {
    grpc::ClientContext context;
    auto stub = SessionService::NewStub(connection_->grpc_channel());
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

  /**
   * Terminate job queue first so that no additional work is enqueued to
   * grpc_queue after termination.
   */
  job_queue_.Terminate();
  if (grpc_queue_) grpc_queue_->Terminate();

  if (connection_) connection_->Disable();

  if (pipe_[0] != 0) {
    close(pipe_[0]);
    close(pipe_[1]);
  }

  if (control_event_source_) {
    loop_->RemoveFd(control_event_source_);
  }
}

void
Session::StartKeepalive()
{
  auto job = CreateJob([id = id_, connection = connection_,
                           grpc_queue = grpc_queue_](bool cancel) {
    if (cancel) {
      return;
    }

    auto context = std::make_unique<grpc::ClientContext>();
    auto stub = SessionService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&SessionService::Stub::PrepareAsyncKeepalive>(
            std::move(stub), std::move(context),
            [connection](SessionTerminateResponse* /*response*/,
                grpc::Status* /*status*/) {
              connection->NotifyDisconnection();
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  job_queue_.Push(std::move(job));
}

bool
Session::Connect(std::shared_ptr<IPeer> peer)
{
  if (pipe2(pipe_, O_CLOEXEC | O_NONBLOCK) == -1) return false;

  auto host_port = peer->host() + ":" + std::to_string(kGrpcPort);
  connection_ = std::make_shared<SessionConnection>(pipe_[1], host_port);

  grpc_queue_ = std::make_shared<AsyncGrpcQueue>();
  grpc_queue_->Start();

  auto stub = SessionService::NewStub(connection_->grpc_channel());

  grpc::ClientContext context;
  NewSessionRequest request;
  NewSessionResponse response;

  auto status = stub->New(&context, request, &response);

  if (!status.ok()) {
    LOG_DEBUG("Failed to start session: %s", status.error_message().c_str());
    close(pipe_[0]);
    close(pipe_[1]);
    grpc_queue_.reset();
    connection_.reset();
    return false;
  }

  id_ = response.id();

  control_event_source_ = new FdSource();

  control_event_source_->fd = pipe_[0];
  control_event_source_->mask = FdSource::kReadable;
  control_event_source_->callback = [this](int fd, uint32_t /*mask*/) {
    SessionConnection::ControlMessage message = SessionConnection::kError;
    read(fd, &message, sizeof(message));

    HandleControlEvent(message);  // this may destroy self
  };

  loop_->AddFd(control_event_source_);

  connected_ = true;

  job_queue_.StartWorkerThread();

  StartKeepalive();

  return true;
}

int32_t
Session::GetPendingGrpcQueueCount()
{
  return grpc_queue_->pending_count();
}

void
Session::HandleControlEvent(SessionConnection::ControlMessage message)
{
  switch (message) {
    case SessionConnection::kDisconnect:  // fall through
    case SessionConnection::kError:
      if (connected_ == false) return;
      connected_ = false;
      this->connection_->Disable();
      this->on_disconnect();  // this may destroy self
      return;

    default:
      break;
  }
}

uint64_t
Session::NewSerial(Session::SerialType type)
{
  return serials_[type]++;
}

std::unique_ptr<ISession>
CreateSession(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Session>(std::move(loop));
}

}  // namespace zen::remote::server
