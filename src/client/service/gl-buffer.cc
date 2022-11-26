#include "client/service/gl-buffer.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

GlBufferServiceImpl::GlBufferServiceImpl(Remote* remote) : remote_(remote) {}

void
GlBufferServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlBufferServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&GlBufferService::AsyncService::RequestNew,
      &GlBufferServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&GlBufferService::AsyncService::RequestDelete,
      &GlBufferServiceImpl::Delete>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&GlBufferService::AsyncService::RequestGlBufferData,
      &GlBufferServiceImpl::GlBufferData>::Listen(&async_, this,
      completion_queue, remote_);
}

grpc::Status
GlBufferServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto gl_buffer = std::make_unique<GlBuffer>(
      request->id(), remote_->update_rendering_queue());

  pool->gl_buffers()->Add(std::move(gl_buffer));

  return grpc::Status::OK;
}

grpc::Status
GlBufferServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  pool->gl_buffers()->ScheduleRemove(request->id());
  return grpc::Status::OK;
}

grpc::Status
GlBufferServiceImpl::GlBufferData(grpc::ServerContext* /*context*/,
    const GlBufferDataRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto gl_buffer = pool->gl_buffers()->Get(request->id());

  gl_buffer->GlBufferData(request->data().data(), request->target(),
      request->data().length(), request->usage());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
