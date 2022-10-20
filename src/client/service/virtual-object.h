#pragma once

#include "core/common.h"
#include "virtual-object.grpc.pb.h"

namespace zen::remote::client {
class ResourcePool;
}

namespace zen::remote::client::service {

class VirtualObjectServiceImpl final : public VirtualObjectService::Service {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObjectServiceImpl);
  VirtualObjectServiceImpl() = delete;
  VirtualObjectServiceImpl(ResourcePool* pool);

  virtual grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  virtual grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  virtual grpc::Status Commit(grpc::ServerContext* context,
      const VirtualObjectCommitRequest* request,
      EmptyResponse* response) override;

 private:
  ResourcePool* pool_;
};

}  // namespace zen::remote::client::service
