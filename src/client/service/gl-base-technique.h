#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "gl-base-technique.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}

namespace zen::remote::client::service {

class GlBaseTechniqueServiceImpl final : public GlBaseTechniqueService::Service,
                                         public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(GlBaseTechniqueServiceImpl);
  GlBaseTechniqueServiceImpl() = delete;
  GlBaseTechniqueServiceImpl(Remote* pool);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewGlBaseTechniqueRequest* request,
      EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status GlDrawArrays(grpc::ServerContext* context,
      const GlDrawArraysRequest* request, EmptyResponse* response) override;

 private:
  GlBaseTechniqueService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
