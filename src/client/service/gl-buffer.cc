#include "client/service/gl-buffer.h"

#include "client/resource-pool.h"
#include "client/service/serial-async-caller.h"

namespace zen::remote::client::service {

GlBufferServiceImpl::GlBufferServiceImpl(ResourcePool* pool) : pool_(pool) {}

void
GlBufferServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlBufferServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue,
    SerialCommandQueue* command_queue)
{
  SerialAsyncCaller<&GlBufferService::AsyncService::RequestNew,
      &GlBufferServiceImpl::New>::Listen(&async_, this, completion_queue,
      command_queue);

  SerialAsyncCaller<&GlBufferService::AsyncService::RequestDelete,
      &GlBufferServiceImpl::Delete>::Listen(&async_, this, completion_queue,
      command_queue);

  SerialAsyncCaller<&GlBufferService::AsyncService::RequestGlBufferData,
      &GlBufferServiceImpl::GlBufferData>::Listen(&async_, this,
      completion_queue, command_queue);
}

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
