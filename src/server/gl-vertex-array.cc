#include "server/gl-vertex-array.h"

#include "core/logger.h"
#include "gl-vertex-array.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

GlVertexArray::GlVertexArray(std::shared_ptr<Session> session)
    : id_(session->NewSerial(Session::kResource)), session_(std::move(session))
{
}

void
GlVertexArray::Init()
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw,
                           grpc_queue = session->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlVertexArrayService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlVertexArrayService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlVertexArray::New");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

GlVertexArray::~GlVertexArray()
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw,
                           grpc_queue = session->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlVertexArrayService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlVertexArrayService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlVertexArray::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

std::unique_ptr<IGlVertexArray>
CreateGlVertexArray(std::shared_ptr<ISession> session)
{
  auto gl_buffer = std::make_unique<GlVertexArray>(
      std::dynamic_pointer_cast<Session>(session));
  gl_buffer->Init();

  return gl_buffer;
}

}  // namespace zen::remote::server
