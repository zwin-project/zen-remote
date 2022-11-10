#include "server/virtual-object.h"

#include "core/logger.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"
#include "virtual-object.grpc.pb.h"

namespace zen::remote::server {

VirtualObject::VirtualObject(std::shared_ptr<Session> session)
    : session_(std::move(session)), id_(session_->NewSerial(Session::kResource))
{
}

void
VirtualObject::Init()
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw,
                           grpc_queue = session_->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = VirtualObjectService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&VirtualObjectService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::New");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

void
VirtualObject::Commit()
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw,
                           grpc_queue = session_->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = VirtualObjectService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&VirtualObjectService::Stub::PrepareAsyncCommit>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::Commit");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

VirtualObject::~VirtualObject()
{
  auto context_raw = new SerialRequestContext(session_.get());

  auto job = CreateJob([id = id_, connection = session_->connection(),
                           context_raw,
                           grpc_queue = session_->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = VirtualObjectService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&VirtualObjectService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session_->job_queue()->Push(std::move(job));
}

uint64_t
VirtualObject::id()
{
  return id_;
}

std::unique_ptr<IVirtualObject>
CreateVirtualObject(std::shared_ptr<ISession> session)
{
  auto virtual_object = std::make_unique<VirtualObject>(
      std::dynamic_pointer_cast<Session>(session));

  virtual_object->Init();

  return virtual_object;
}

}  // namespace zen::remote::server
