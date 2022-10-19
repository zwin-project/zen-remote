#include "client/service/rendering-unit.h"

#include "client/resource-pool.h"

namespace zen::remote::client::service {

RenderingUnitServiceImpl::RenderingUnitServiceImpl(ResourcePool* pool)
    : pool_(pool)
{
}

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

grpc::Status
RenderingUnitServiceImpl::Commit(grpc::ServerContext* /*context*/,
    const RenderingUnitCommitRequest* /*request*/, EmptyResponse* /*response*/)
{
  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::GlEnableVertexAttribArray(
    grpc::ServerContext* /*context*/,
    const GlEnableVertexAttribArrayRequest* /*request*/,
    EmptyResponse* /*response*/)
{
  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::GlDisableVertexAttribArray(
    grpc::ServerContext* /*context*/,
    const GlDisableVertexAttribArrayRequest* /*request*/,
    EmptyResponse* /*response*/)
{
  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::GlVertexAttribPointer(
    grpc::ServerContext* /*context*/,
    const GlVertexAttribPointerRequest* /*request*/,
    EmptyResponse* /*response*/)
{
  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
