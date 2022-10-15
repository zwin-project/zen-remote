#pragma once

#include "rendering-unit.grpc.pb.h"

namespace zen::remote::client::service {

class RenderingUnitServiceImpl final : public RenderingUnitService::Service {
  virtual grpc::Status New(grpc::ServerContext* context,
      const NewObjectRequest* request, EmptyResponse* response) override;

  virtual grpc::Status Delete(grpc::ServerContext* context,
      const DeleteObjectRequest* request, EmptyResponse* response) override;
};

}  // namespace zen::remote::client::service
