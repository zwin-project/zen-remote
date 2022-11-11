#include "server/gl-base-technique.h"

#include "core/logger.h"
#include "gl-base-technique.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

GlBaseTechnique::GlBaseTechnique(std::shared_ptr<Session> session)
    : session_(std::move(session)), id_(session_->NewSerial(Session::kResource))
{
}

void
GlBaseTechnique::Init(uint64_t rendering_unit_id)
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw, grpc_queue = session_->grpc_queue(),
                           rendering_unit_id](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBaseTechniqueService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::New");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);
    caller->request()->set_rendering_unit_id(rendering_unit_id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw, grpc_queue = session_->grpc_queue(),
                           mode, first, count](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlBaseTechniqueService::Stub::PrepareAsyncGlDrawArrays>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBaseTechnique::GlDrawArrays");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_mode(mode);
    caller->request()->set_first(first);
    caller->request()->set_count(count);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

GlBaseTechnique::~GlBaseTechnique()
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw,
                           grpc_queue = session_->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBaseTechniqueService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

std::unique_ptr<IGlBaseTechnique>
CreateGlBaseTechnique(
    std::shared_ptr<ISession> session, uint64_t rendering_unit_id)
{
  auto gl_base_technique = std::make_unique<GlBaseTechnique>(
      std::dynamic_pointer_cast<Session>(session));

  gl_base_technique->Init(rendering_unit_id);

  return gl_base_technique;
}

}  // namespace zen::remote::server
