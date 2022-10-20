#include "client/service/virtual-object.h"

#include "client/resource-pool.h"

namespace zen::remote::client::service {

VirtualObjectServiceImpl::VirtualObjectServiceImpl(ResourcePool* pool)
    : pool_(pool)
{
}

grpc::Status
VirtualObjectServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto virtual_object = std::make_unique<VirtualObject>(request->id());

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
    const VirtualObjectCommitRequest* /*request*/, EmptyResponse* /*response*/)
{
  // TODO:
  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
