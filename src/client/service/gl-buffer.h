#pragma once

#include "core/common.h"
#include "gl-buffer.grpc.pb.h"

namespace zen::remote::client {
class ResourcePool;
}

namespace zen::remote::client::service {

class GlBufferServiceImpl final : public GlBufferService::Service {
 public:
  DISABLE_MOVE_AND_COPY(GlBufferServiceImpl);
  GlBufferServiceImpl() = delete;
  GlBufferServiceImpl(ResourcePool* pool);

  virtual grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  virtual grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

 private:
  ResourcePool* pool_;
};

}  // namespace zen::remote::client::service
