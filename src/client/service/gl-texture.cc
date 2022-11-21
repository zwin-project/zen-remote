#include "client/service/gl-texture.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

GlTextureServiceImpl::GlTextureServiceImpl(Remote* remote) : remote_(remote) {}

void
GlTextureServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlTextureServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&GlTextureService::AsyncService::RequestNew,
      &GlTextureServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&GlTextureService::AsyncService::RequestDelete,
      &GlTextureServiceImpl::Delete>::Listen(&async_, this, completion_queue,
      remote_);
}

grpc::Status
GlTextureServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto gl_texture = std::make_unique<GlTexture>(
      request->id(), remote_->update_rendering_queue());

  pool->gl_textures()->Add(std::move(gl_texture));

  return grpc::Status::OK;
}

grpc::Status
GlTextureServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  pool->gl_textures()->ScheduleRemove(request->id());

  return grpc::Status::OK;
}

grpc::Status
GlTextureServiceImpl::GlTexImage2D(grpc::ServerContext* /*context*/,
    const GlTexImage2DRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();
  auto texture = pool->gl_textures()->Get(request->id());

  texture->GlTexImage2D(request->target(), request->level(),
      request->internal_format(), request->width(), request->height(),
      request->border(), request->format(), request->type(),
      request->data().size(), request->data().c_str());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
