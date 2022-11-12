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
  StopPingThread();

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

  id_ = response.id();

  StartPingThread();

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

  return true;
}

/**
 * Excuse: This is a stopgap implementation to prioritize other parts of the
 * development. It can use keepalive of gRPC or be more concise implementation.
 */
void
Session::StartPingThread()
{
  if (ping_thread_.joinable()) return;

  should_ping_ = true;

  ping_thread_ = std::thread([this, id = id_, connection = connection_] {
    auto cq = std::make_shared<grpc::CompletionQueue>();

    auto stub = SessionService::NewStub(connection->grpc_channel());

    grpc::ClientContext context;
    grpc::Status status;
    SessionTerminateResponse response;

    bool finished = false;
    std::mutex finish_mutex;
    std::condition_variable finish_cond;

    auto writer = stub->AsyncPing(&context, &response, cq.get(), NULL);

    auto completion_thread =
        std::thread([cq, &finish_mutex, &finish_cond, &finished, connection] {
          void *tag;
          bool ok = false;

          while (cq->Next(&tag, &ok)) {
            if (!ok) {
              connection->NotifyDisconnection();
            }
            if ((bool)tag) {  // finished
              {
                std::lock_guard<std::mutex> lock(finish_mutex);
                finished = true;
              }
              finish_cond.notify_one();
            }
          }
        });

    SessionPingRequest request;
    SessionPingRequest done_request;

    writer->Finish(&status, (void *)true);

    int i = 0;
    for (;;) {
      i++;
      std::unique_lock<std::mutex> lock(ping_mutex_);

      {
        request.set_id(id);
        request.set_done(false);
        writer->Write(request, (void *)false);
      }

      ping_cond_.wait_for(lock, std::chrono::seconds(1),
          [this] { return should_ping_ == false; });

      if (should_ping_ == false) {
        done_request.set_id(id);
        done_request.set_done(true);
        writer->Write(done_request, (void *)false);
        break;
      };
    }

    std::unique_lock<std::mutex> lock(finish_mutex);

    finish_cond.wait_for(
        lock, std::chrono::microseconds(100), [&finished] { return finished; });

    cq->Shutdown();

    completion_thread.detach();
  });
}

void
Session::StopPingThread()
{
  {
    std::lock_guard<std::mutex> lock(ping_mutex_);
    should_ping_ = false;
  }

  ping_cond_.notify_one();

  ping_thread_.join();
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
