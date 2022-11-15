#include "client/service/gl-program.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

GlProgramServiceImpl::GlProgramServiceImpl(Remote* remote) : remote_(remote) {}

void
GlProgramServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlProgramServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&GlProgramService::AsyncService::RequestNew,
      &GlProgramServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&GlProgramService::AsyncService::RequestDelete,
      &GlProgramServiceImpl::Delete>::Listen(&async_, this, completion_queue,
      remote_);
}

grpc::Status
GlProgramServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto gl_program = std::make_unique<GlProgram>(
      request->id(), remote_->update_rendering_queue());

  pool->gl_programs()->Add(std::move(gl_program));

  return grpc::Status::OK;
}

grpc::Status
GlProgramServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  pool->gl_programs()->ScheduleRemove(request->id());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
