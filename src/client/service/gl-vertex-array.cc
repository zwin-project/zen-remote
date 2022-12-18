#include "client/service/gl-vertex-array.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

GlVertexArrayServiceImpl::GlVertexArrayServiceImpl(Remote* remote)
    : remote_(remote)
{
}

void
GlVertexArrayServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlVertexArrayServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&GlVertexArrayService::AsyncService::RequestNew,
      &GlVertexArrayServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&GlVertexArrayService::AsyncService::RequestDelete,
      &GlVertexArrayServiceImpl::Delete>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlVertexArrayService::AsyncService::RequestGlEnableVertexAttribArray,
      &GlVertexArrayServiceImpl::GlEnableVertexAttribArray>::Listen(&async_,
      this, completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlVertexArrayService::AsyncService::RequestGlDisableVertexAttribArray,
      &GlVertexArrayServiceImpl::GlDisableVertexAttribArray>::Listen(&async_,
      this, completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlVertexArrayService::AsyncService::RequestGlVertexAttribPointer,
      &GlVertexArrayServiceImpl::GlVertexAttribPointer>::Listen(&async_, this,
      completion_queue, remote_);
}

grpc::Status
GlVertexArrayServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  auto gl_vertex_array = std::make_unique<GlVertexArray>(
      request->id(), remote_->update_rendering_queue());

  pool->gl_vertex_arrays()->Add(std::move(gl_vertex_array));

  return grpc::Status::OK;
}

grpc::Status
GlVertexArrayServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  pool->gl_vertex_arrays()->ScheduleRemove(request->id());

  return grpc::Status::OK;
}

grpc::Status
GlVertexArrayServiceImpl::GlEnableVertexAttribArray(
    grpc::ServerContext* /*context*/,
    const GlEnableVertexAttribArrayRequest* request,
    EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  auto vertex_array = pool->gl_vertex_arrays()->Get(request->id());

  vertex_array->GlEnableVertexAttribArray(request->index());

  return grpc::Status::OK;
}

grpc::Status
GlVertexArrayServiceImpl::GlDisableVertexAttribArray(
    grpc::ServerContext* /*context*/,
    const GlDisableVertexAttribArrayRequest* request,
    EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  auto vertex_array = pool->gl_vertex_arrays()->Get(request->id());

  vertex_array->GlDisableVertexAttribArray(request->index());

  return grpc::Status::OK;
}

grpc::Status
GlVertexArrayServiceImpl::GlVertexAttribPointer(
    grpc::ServerContext* /*context*/,
    const GlVertexAttribPointerRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  auto vertex_array = pool->gl_vertex_arrays()->Get(request->id());
  auto gl_buffer = pool->gl_buffers()->Get(request->gl_buffer_id());

  vertex_array->GlVertexAttribPointer(request->index(), request->size(),
      request->type(), request->normalized(), request->stride(),
      request->offset(), gl_buffer);

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
