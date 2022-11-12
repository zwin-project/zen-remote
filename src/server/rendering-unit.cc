#include "server/rendering-unit.h"

#include "core/logger.h"
#include "rendering-unit.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

RenderingUnit::RenderingUnit(std::shared_ptr<Session> session)
    : id_(session->NewSerial(Session::kResource)), session_(std::move(session))
{
}

void
RenderingUnit::Init(uint64_t virtual_object_id)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, virtual_object_id,
                           connection = session->connection(), context_raw,
                           grpc_queue = session->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = RenderingUnitService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&RenderingUnitService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote RenderingUnit::New");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);
    caller->request()->set_virtual_object_id(virtual_object_id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

RenderingUnit::~RenderingUnit()
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

    auto stub = RenderingUnitService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&RenderingUnitService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote RenderingUnit::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

uint64_t
RenderingUnit::id()
{
  return id_;
}

std::unique_ptr<IRenderingUnit>
CreateRenderingUnit(
    std::shared_ptr<ISession> session, uint64_t virtual_object_id)
{
  auto rendering_unit = std::make_unique<RenderingUnit>(
      std::dynamic_pointer_cast<Session>(session));

  rendering_unit->Init(virtual_object_id);

  return rendering_unit;
}

}  // namespace zen::remote::server
