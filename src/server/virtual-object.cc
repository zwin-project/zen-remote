#include "server/virtual-object.h"

#include "core/logger.h"
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
  auto context = new SerialRequestContext(session_.get());

  auto job = CreateJob(
      [id = id_, channel = session_->grpc_channel(), context](bool cancel) {
        if (cancel) {
          delete context;
          return;
        }

        auto stub = VirtualObjectService::NewStub(channel);

        auto request = new NewResourceRequest();
        auto response = new EmptyResponse();

        request->set_id(id);

        stub->async()->New(context, request, response,
            [context, request, response](grpc::Status status) {
              if (!status.ok() && status.error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::New");
              }
              delete context;
              delete request;
              delete response;
            });
      });

  session_->job_queue()->Push(std::move(job));
}

void
VirtualObject::Commit()
{
  auto context = new SerialRequestContext(session_.get());

  auto job = CreateJob(
      [id = id_, channel = session_->grpc_channel(), context](bool cancel) {
        if (cancel) {
          delete context;
          return;
        }

        auto stub = VirtualObjectService::NewStub(channel);

        auto request = new VirtualObjectCommitRequest();
        auto response = new EmptyResponse();

        request->set_id(id);

        stub->async()->Commit(context, request, response,
            [context, request, response](grpc::Status status) {
              if (!status.ok() && status.error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::Commit");
              }
              delete context;
              delete request;
              delete response;
            });
      });

  session_->job_queue()->Push(std::move(job));
}

VirtualObject::~VirtualObject()
{
  auto context = new SerialRequestContext(session_.get());

  auto job = CreateJob(
      [id = id_, channel = session_->grpc_channel(), context](bool cancel) {
        if (cancel) {
          delete context;
          return;
        }

        auto stub = VirtualObjectService::NewStub(channel);

        auto request = new DeleteResourceRequest();
        auto response = new EmptyResponse();

        request->set_id(id);

        stub->async()->Delete(context, request, response,
            [context, request, response](grpc::Status status) {
              if (!status.ok() && status.error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::Delete");
              }
              delete context;
              delete request;
              delete response;
            });
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
