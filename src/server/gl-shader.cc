#include "server/gl-shader.h"

#include "core/logger.h"
#include "gl-shader.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

GlShader::GlShader(std::shared_ptr<Session> session)
    : id_(session->NewSerial(Session::kResource)), session_(std::move(session))
{
}

void
GlShader::Init(std::string source, uint32_t type)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           source = std::move(source), type](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlShaderService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<&GlShaderService::Stub::PrepareAsyncNew>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlShader::New");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_source(std::move(source));
    caller->request()->set_type(type);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

GlShader::~GlShader()
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

    auto stub = GlShaderService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlShaderService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlShader::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

uint64_t
GlShader::id()
{
  return id_;
}

std::unique_ptr<IGlShader>
CreateGlShader(
    std::shared_ptr<ISession> session, std::string source, uint32_t type)
{
  auto gl_shader =
      std::make_unique<GlShader>(std::dynamic_pointer_cast<Session>(session));

  gl_shader->Init(std::move(source), type);

  return gl_shader;
}

}  // namespace zen::remote::server
