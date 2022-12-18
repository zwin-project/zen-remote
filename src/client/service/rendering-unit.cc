#include "client/service/rendering-unit.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

RenderingUnitServiceImpl::RenderingUnitServiceImpl(Remote* remote)
    : remote_(remote)
{
}

void
RenderingUnitServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
RenderingUnitServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&RenderingUnitService::AsyncService::RequestNew,
      &RenderingUnitServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&RenderingUnitService::AsyncService::RequestDelete,
      &RenderingUnitServiceImpl::Delete>::Listen(&async_, this,
      completion_queue, remote_);
}

grpc::Status
RenderingUnitServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewRenderingUnitRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  auto rendering_unit = std::make_shared<RenderingUnit>(
      request->id(), remote_->update_rendering_queue());
  auto virtual_object =
      pool->virtual_objects()->Get(request->virtual_object_id());

  virtual_object->AddRenderingUnit(rendering_unit);

  pool->rendering_units()->Add(std::move(rendering_unit));

  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->current()->pool();

  pool->rendering_units()->ScheduleRemove(request->id());
  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
