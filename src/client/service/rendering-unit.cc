#include "client/service/rendering-unit.h"

#include "client/resource-pool.h"
#include "client/service/serial-async-caller.h"

namespace zen::remote::client::service {

RenderingUnitServiceImpl::RenderingUnitServiceImpl(ResourcePool* pool)
    : pool_(pool)
{
}

void
RenderingUnitServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
RenderingUnitServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue,
    SerialCommandQueue* command_queue)
{
  SerialAsyncCaller<&RenderingUnitService::AsyncService::RequestNew,
      &RenderingUnitServiceImpl::New>::Listen(&async_, this, completion_queue,
      command_queue);

  SerialAsyncCaller<&RenderingUnitService::AsyncService::RequestDelete,
      &RenderingUnitServiceImpl::Delete>::Listen(&async_, this,
      completion_queue, command_queue);

  SerialAsyncCaller<
      &RenderingUnitService::AsyncService::RequestGlEnableVertexAttribArray,
      &RenderingUnitServiceImpl::GlEnableVertexAttribArray>::Listen(&async_,
      this, completion_queue, command_queue);

  SerialAsyncCaller<
      &RenderingUnitService::AsyncService::RequestGlDisableVertexAttribArray,
      &RenderingUnitServiceImpl::GlDisableVertexAttribArray>::Listen(&async_,
      this, completion_queue, command_queue);

  SerialAsyncCaller<
      &RenderingUnitService::AsyncService::RequestGlVertexAttribPointer,
      &RenderingUnitServiceImpl::GlVertexAttribPointer>::Listen(&async_, this,
      completion_queue, command_queue);
}

grpc::Status
RenderingUnitServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewRenderingUnitRequest* request, EmptyResponse* /*response*/)
{
  auto rendering_unit = std::make_shared<RenderingUnit>(
      request->id(), pool_->update_rendering_queue());
  auto virtual_object =
      pool_->virtual_objects()->Get(request->virtual_object_id());

  virtual_object->AddRenderingUnit(rendering_unit);

  pool_->rendering_units()->Add(std::move(rendering_unit));

  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  pool_->rendering_units()->ScheduleRemove(request->id());
  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::GlEnableVertexAttribArray(
    grpc::ServerContext* /*context*/,
    const GlEnableVertexAttribArrayRequest* request,
    EmptyResponse* /*response*/)
{
  auto rendering_unit = pool_->rendering_units()->Get(request->id());

  rendering_unit->GlEnableVertexAttribArray(request->index());

  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::GlDisableVertexAttribArray(
    grpc::ServerContext* /*context*/,
    const GlDisableVertexAttribArrayRequest* request,
    EmptyResponse* /*response*/)
{
  auto rendering_unit = pool_->rendering_units()->Get(request->id());

  rendering_unit->GlDisableVertexAttribArray(request->index());

  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::GlVertexAttribPointer(
    grpc::ServerContext* /*context*/,
    const GlVertexAttribPointerRequest* request, EmptyResponse* /*response*/)
{
  auto rendering_unit = pool_->rendering_units()->Get(request->id());
  auto gl_buffer = pool_->gl_buffers()->Get(request->buffer_id());

  rendering_unit->GlVertexAttribPointer(request->index(), gl_buffer,
      request->size(), request->type(), request->normalized(),
      request->stride(), request->offset());

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
