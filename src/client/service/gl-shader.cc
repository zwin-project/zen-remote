#include "client/service/gl-shader.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

GlShaderServiceImpl::GlShaderServiceImpl(Remote* remote) : remote_(remote) {}

void
GlShaderServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlShaderServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&GlShaderService::AsyncService::RequestNew,
      &GlShaderServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&GlShaderService::AsyncService::RequestDelete,
      &GlShaderServiceImpl::Delete>::Listen(&async_, this, completion_queue,
      remote_);
}

grpc::Status
GlShaderServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewGlShaderRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  auto gl_shader = std::make_unique<GlShader>(request->id(), request->source(),
      request->type(), remote_->update_rendering_queue());

  pool->gl_shaders()->Add(std::move(gl_shader));

  return grpc::Status::OK;
}

grpc::Status
GlShaderServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  pool->gl_shaders()->ScheduleRemove(request->id());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
