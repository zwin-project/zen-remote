#include "client/service/virtual-object.h"

#include "client/resource-pool.h"
#include "client/service/serial-async-caller.h"

namespace zen::remote::client::service {

VirtualObjectServiceImpl::VirtualObjectServiceImpl(ResourcePool* pool)
    : pool_(pool)
{
}

void
VirtualObjectServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
VirtualObjectServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue,
    SerialCommandQueue* command_queue)
{
  SerialAsyncCaller<&VirtualObjectService::AsyncService::RequestNew,
      &VirtualObjectServiceImpl::New>::Listen(&async_, this, completion_queue,
      command_queue);

  SerialAsyncCaller<&VirtualObjectService::AsyncService::RequestDelete,
      &VirtualObjectServiceImpl::Delete>::Listen(&async_, this,
      completion_queue, command_queue);

  SerialAsyncCaller<&VirtualObjectService::AsyncService::RequestCommit,
      &VirtualObjectServiceImpl::Commit>::Listen(&async_, this,
      completion_queue, command_queue);
}

grpc::Status
VirtualObjectServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto virtual_object = std::make_unique<VirtualObject>(
      request->id(), pool_->update_rendering_queue());

  pool_->virtual_objects()->Add(std::move(virtual_object));

  return grpc::Status::OK;
}

grpc::Status
VirtualObjectServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  pool_->virtual_objects()->ScheduleRemove(request->id());

  return grpc::Status::OK;
}

grpc::Status
VirtualObjectServiceImpl::Commit(grpc::ServerContext* /*context*/,
    const VirtualObjectCommitRequest* request, EmptyResponse* /*response*/)
{
  auto virtual_object = pool_->virtual_objects()->Get(request->id());

  virtual_object->Commit();

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
