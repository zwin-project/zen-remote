#include "server/virtual-object.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/remote.h"
#include "virtual-object.grpc.pb.h"

namespace zen::remote::server {

VirtualObject::VirtualObject(std::shared_ptr<Remote> remote)
    : remote_(std::move(remote)),
      id_(remote_->NewSerial(Remote::SerialType::kResource))
{
}

void
VirtualObject::Init()
{
  auto job = CreateJob([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = VirtualObjectService::NewStub(channel);

    auto context = new grpc::ClientContext();
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

  remote_->job_queue()->Push(std::move(job));
}

void
VirtualObject::Commit()
{
  auto job = CreateJob([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = VirtualObjectService::NewStub(channel);

    auto context = new grpc::ClientContext();
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

  remote_->job_queue()->Push(std::move(job));
}

VirtualObject::~VirtualObject()
{
  auto job = CreateJob([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = VirtualObjectService::NewStub(channel);

    auto context = new grpc::ClientContext();
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

  remote_->job_queue()->Push(std::move(job));
}

uint64_t
VirtualObject::id()
{
  return id_;
}

std::unique_ptr<IVirtualObject>
CreateVirtualObject(std::shared_ptr<IRemote> remote)
{
  auto virtual_object = std::make_unique<VirtualObject>(
      std::dynamic_pointer_cast<Remote>(remote));

  virtual_object->Init();

  return virtual_object;
}

}  // namespace zen::remote::server
