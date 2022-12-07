#include "client/service/gl-sampler.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

GlSamplerServiceImpl::GlSamplerServiceImpl(Remote* remote) : remote_(remote) {}

void
GlSamplerServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlSamplerServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&GlSamplerService::AsyncService::RequestNew,
      &GlSamplerServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&GlSamplerService::AsyncService::RequestDelete,
      &GlSamplerServiceImpl::Delete>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<
      &GlSamplerService::AsyncService::RequestGlSamplerParameterf,
      &GlSamplerServiceImpl::GlSamplerParameterf>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlSamplerService::AsyncService::RequestGlSamplerParameteri,
      &GlSamplerServiceImpl::GlSamplerParameteri>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlSamplerService::AsyncService::RequestGlSamplerParameterfv,
      &GlSamplerServiceImpl::GlSamplerParameterfv>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlSamplerService::AsyncService::RequestGlSamplerParameteriv,
      &GlSamplerServiceImpl::GlSamplerParameteriv>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlSamplerService::AsyncService::RequestGlSamplerParameterIiv,
      &GlSamplerServiceImpl::GlSamplerParameterIiv>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<
      &GlSamplerService::AsyncService::RequestGlSamplerParameterIuiv,
      &GlSamplerServiceImpl::GlSamplerParameterIuiv>::Listen(&async_, this,
      completion_queue, remote_);
}

grpc::Status
GlSamplerServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto gl_sampler = std::make_unique<GlSampler>(
      request->id(), remote_->update_rendering_queue());

  pool->gl_samplers()->Add(std::move(gl_sampler));

  return grpc::Status::OK;
}

grpc::Status
GlSamplerServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  pool->gl_samplers()->ScheduleRemove(request->id());

  return grpc::Status::OK;
}

grpc::Status
GlSamplerServiceImpl::GlSamplerParameterf(grpc::ServerContext* /*context*/,
    const GlSamplerParameterfRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();
  auto sampler = pool->gl_samplers()->Get(request->id());

  sampler->Parameterf(request->pname(), request->param());

  return grpc::Status::OK;
}

grpc::Status
GlSamplerServiceImpl::GlSamplerParameteri(grpc::ServerContext* /*context*/,
    const GlSamplerParameteriRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();
  auto sampler = pool->gl_samplers()->Get(request->id());

  sampler->Parameteri(request->pname(), request->param());

  return grpc::Status::OK;
}

grpc::Status
GlSamplerServiceImpl::GlSamplerParameterfv(grpc::ServerContext* /*context*/,
    const GlSamplerParametervRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();
  auto sampler = pool->gl_samplers()->Get(request->id());

  sampler->Parameterfv(request->pname(), request->params());

  return grpc::Status::OK;
}

grpc::Status
GlSamplerServiceImpl::GlSamplerParameteriv(grpc::ServerContext* /*context*/,
    const GlSamplerParametervRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();
  auto sampler = pool->gl_samplers()->Get(request->id());

  sampler->Parameteriv(request->pname(), request->params());

  return grpc::Status::OK;
}

grpc::Status
GlSamplerServiceImpl::GlSamplerParameterIiv(grpc::ServerContext* /*context*/,
    const GlSamplerParametervRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();
  auto sampler = pool->gl_samplers()->Get(request->id());

  sampler->ParameterIiv(request->pname(), request->params());

  return grpc::Status::OK;
}

grpc::Status
GlSamplerServiceImpl::GlSamplerParameterIuiv(grpc::ServerContext* /*context*/,
    const GlSamplerParametervRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();
  auto sampler = pool->gl_samplers()->Get(request->id());

  sampler->ParameterIuiv(request->pname(), request->params());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
