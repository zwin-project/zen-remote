#include "server/gl-buffer.h"

#include "core/logger.h"
#include "gl-buffer.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/buffer.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

GlBuffer::GlBuffer(std::shared_ptr<Session> session)
    : session_(std::move(session)), id_(session_->NewSerial(Session::kResource))
{
}

void
GlBuffer::Init()
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw,
                           grpc_queue = session_->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBufferService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<&GlBufferService::Stub::PrepareAsyncNew>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBuffer::New");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

void
GlBuffer::GlBufferData(std::unique_ptr<IBuffer> buffer, uint64_t target,
    size_t size, uint64_t usage)
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw, grpc_queue = session_->grpc_queue(),
                           buffer = std::move(buffer), target, size,
                           usage](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBufferService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBufferService::Stub::PrepareAsyncGlBufferData>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBuffer::GlBufferData");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);
    caller->request()->set_target(target);
    caller->request()->set_usage(usage);
    caller->request()->set_data(buffer->data(), size);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

GlBuffer::~GlBuffer()
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw,
                           grpc_queue = session_->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBufferService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBufferService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBuffer::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

uint64_t
GlBuffer::id()
{
  return id_;
}

std::unique_ptr<IGlBuffer>
CreateGlBuffer(std::shared_ptr<ISession> session)
{
  auto gl_buffer =
      std::make_unique<GlBuffer>(std::dynamic_pointer_cast<Session>(session));

  gl_buffer->Init();

  return gl_buffer;
}

}  // namespace zen::remote::server
