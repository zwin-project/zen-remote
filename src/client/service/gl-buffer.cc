#include "client/service/gl-buffer.h"

#include "client/resource-pool.h"

namespace zen::remote::client::service {

GlBufferServiceImpl::GlBufferServiceImpl(ResourcePool* pool) : pool_(pool) {}

grpc::Status
GlBufferServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* /*request*/, EmptyResponse* /*response*/)
{
  return grpc::Status::OK;
}

grpc::Status
GlBufferServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* /*request*/, EmptyResponse* /*response*/)
{
  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
