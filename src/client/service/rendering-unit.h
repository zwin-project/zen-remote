#pragma once

#include "client/resource-pool.h"
#include "core/common.h"
#include "rendering-unit.grpc.pb.h"

namespace zen::remote::client::service {

class RenderingUnitServiceImpl final : public RenderingUnitService::Service {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnitServiceImpl);
  RenderingUnitServiceImpl() = delete;
  RenderingUnitServiceImpl(std::shared_ptr<ResourcePool> pool)
      : pool_(std::move(pool)){};

 private:
  virtual grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  virtual grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  std::shared_ptr<ResourcePool> pool_;
};

}  // namespace zen::remote::client::service
