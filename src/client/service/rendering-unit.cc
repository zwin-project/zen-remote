#include "client/service/rendering-unit.h"

#include "core/logger.h"

namespace zen::remote::client::service {

grpc::Status
RenderingUnitServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto rendering_unit = std::make_unique<RenderingUnit>(request->id());

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

}  // namespace zen::remote::client::service
