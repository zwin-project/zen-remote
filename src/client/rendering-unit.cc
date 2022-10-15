#include "client/rendering-unit.h"

#include "core/logger.h"

namespace zen::remote::client {

grpc::Status
RenderingUnitServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewObjectRequest* request, EmptyResponse* /*response*/)
{
  LOG_DEBUG("New rendering unit: %ld", request->id());  // FIXME:
  return grpc::Status::OK;
}

grpc::Status
RenderingUnitServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteObjectRequest* request, EmptyResponse* /*response*/)
{
  LOG_DEBUG("Delete rendering unit: %ld", request->id());  // FIXME:
  return grpc::Status::OK;
}

}  // namespace zen::remote::client
