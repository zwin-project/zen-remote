#include "client/service/gl-base-technique.h"

#include "client/resource-pool.h"
#include "client/service/serial-async-caller.h"

namespace zen::remote::client::service {

GlBaseTechniqueServiceImpl::GlBaseTechniqueServiceImpl(ResourcePool* pool)
    : pool_(pool)
{
}

void
GlBaseTechniqueServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
GlBaseTechniqueServiceImpl::Listen(
    grpc::ServerCompletionQueue* completion_queue,
    SerialCommandQueue* command_queue)
{
  SerialAsyncCaller<&GlBaseTechniqueService::AsyncService::RequestNew,
      &GlBaseTechniqueServiceImpl::New>::Listen(&async_, this, completion_queue,
      command_queue);

  SerialAsyncCaller<&GlBaseTechniqueService::AsyncService::RequestDelete,
      &GlBaseTechniqueServiceImpl::Delete>::Listen(&async_, this,
      completion_queue, command_queue);

  SerialAsyncCaller<&GlBaseTechniqueService::AsyncService::RequestGlDrawArrays,
      &GlBaseTechniqueServiceImpl::GlDrawArrays>::Listen(&async_, this,
      completion_queue, command_queue);
}

grpc::Status
GlBaseTechniqueServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewGlBaseTechniqueRequest* request, EmptyResponse* /*response*/)
{
  auto gl_base_technique = std::make_shared<GlBaseTechnique>(
      request->id(), pool_->update_rendering_queue());
  auto rendering_unit =
      pool_->rendering_units()->Get(request->rendering_unit_id());

  rendering_unit->SetGlBaseTechnique(gl_base_technique);

  pool_->gl_base_techniques()->Add(std::move(gl_base_technique));

  return grpc::Status::OK;
}

grpc::Status
GlBaseTechniqueServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  pool_->gl_base_techniques()->ScheduleRemove(request->id());
  return grpc::Status::OK;
}

grpc::Status
GlBaseTechniqueServiceImpl::GlDrawArrays(grpc::ServerContext* /*context*/,
    const GlDrawArraysRequest* request, EmptyResponse* /*response*/)
{
  auto base_technique = pool_->gl_base_techniques()->Get(request->id());

  base_technique->GlDrawArrays(
      request->mode(), request->first(), request->count());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
