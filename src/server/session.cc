#include "server/session.h"

#include "core/logger.h"
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
  /**
   * Terminate job queue first so that no additional work is enqueued to
   * grpc_queue after termination.
   */
  job_queue_.Terminate();
  grpc_queue_->Terminate();

  if (connection_) connection_->Disable();

  if (pipe_[0] != 0) {
    close(pipe_[0]);
    close(pipe_[1]);
  }

  if (control_event_source_) {
    loop_->RemoveFd(control_event_source_);
  }
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

  control_event_source_ = new FdSource();

  control_event_source_->fd = pipe_[0];
  control_event_source_->mask = FdSource::kReadable;
  control_event_source_->callback = [this](int /*fd*/, uint32_t /*mask*/) {
    if (connected_ == false) return;
    connected_ = false;
    this->connection_->Disable();
    this->on_disconnect();  // this may destroy self
  };

  loop_->AddFd(control_event_source_);

  id_ = response.id();
  connected_ = true;

  job_queue_.StartWorkerThread();

  return true;
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
