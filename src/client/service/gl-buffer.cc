#include "client/service/gl-buffer.h"

#include "client/resource-pool.h"

namespace zen::remote::client::service {

GlBufferServiceImpl::GlBufferServiceImpl(ResourcePool* pool) : pool_(pool) {}

grpc::Status
GlBufferServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto gl_buffer = std::make_unique<GlBuffer>(request->id());

  pool_->gl_buffers()->Add(std::move(gl_buffer));

  return grpc::Status::OK;
}

grpc::Status
GlBufferServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  pool_->gl_buffers()->ScheduleRemove(request->id());
  return grpc::Status::OK;
}

grpc::Status
GlBufferServiceImpl::GlBufferData(grpc::ServerContext* /*context*/,
    const GlBufferDataRequest* request, EmptyResponse* /*response*/)
{
  auto gl_buffer = pool_->gl_buffers()->Get(request->id());

  gl_buffer->GlBufferData(
      request->data().c_str(), request->data().length(), request->usage());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
