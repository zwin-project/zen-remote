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
  uint64_t id = id_;
  auto remote = remote_;

  auto job = std::make_unique<Job>([id, remote](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = VirtualObjectService::NewStub(channel);

    NewResourceRequest request;
    EmptyResponse response;
    grpc::ClientContext context;

    request.set_id(id);

    auto status = stub->New(&context, request, &response);
    if (!status.ok()) {
      LOG_WARN("Failed to create a new virtual object");
    }
  });

  remote_->job_queue()->Push(std::move(job));
}

VirtualObject::~VirtualObject()
{
  uint64_t id = id_;
  auto remote = remote_;

  auto job = std::make_unique<Job>([id, remote](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = VirtualObjectService::NewStub(channel);

    DeleteResourceRequest request;
    EmptyResponse response;
    grpc::ClientContext context;

    request.set_id(id);

    auto status = stub->Delete(&context, request, &response);
    if (!status.ok()) {
      LOG_WARN("Failed to destroy a new virtual object");
    }
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
