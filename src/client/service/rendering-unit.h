#pragma once

#include "core/common.h"
#include "rendering-unit.grpc.pb.h"

namespace zen::remote::client {
class ResourcePool;
}

namespace zen::remote::client::service {

class RenderingUnitServiceImpl final : public RenderingUnitService::Service {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnitServiceImpl);
  RenderingUnitServiceImpl() = delete;
  RenderingUnitServiceImpl(ResourcePool* pool);

  virtual grpc::Status New(grpc::ServerContext* context,
      const NewRenderingUnitRequest* request, EmptyResponse* response) override;

  virtual grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  virtual grpc::Status GlEnableVertexAttribArray(grpc::ServerContext* context,
      const GlEnableVertexAttribArrayRequest* request,
      EmptyResponse* response) override;

  virtual grpc::Status GlDisableVertexAttribArray(grpc::ServerContext* context,
      const GlDisableVertexAttribArrayRequest* request,
      EmptyResponse* response) override;

  virtual grpc::Status GlVertexAttribPointer(grpc::ServerContext* context,
      const GlVertexAttribPointerRequest* request,
      EmptyResponse* response) override;

 private:
  ResourcePool* pool_;
};

}  // namespace zen::remote::client::service
