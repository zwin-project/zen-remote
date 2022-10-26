#pragma once

#include "client/service/serial-async-service.h"
#include "core/common.h"
#include "rendering-unit.grpc.pb.h"

namespace zen::remote::client {
class ResourcePool;
}

namespace zen::remote::client::service {

class RenderingUnitServiceImpl final : public RenderingUnitService::Service,
                                       public ISerialAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnitServiceImpl);
  RenderingUnitServiceImpl() = delete;
  RenderingUnitServiceImpl(ResourcePool* pool);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue,
      SerialCommandQueue* command_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewRenderingUnitRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status GlEnableVertexAttribArray(grpc::ServerContext* context,
      const GlEnableVertexAttribArrayRequest* request,
      EmptyResponse* response) override;

  grpc::Status GlDisableVertexAttribArray(grpc::ServerContext* context,
      const GlDisableVertexAttribArrayRequest* request,
      EmptyResponse* response) override;

  grpc::Status GlVertexAttribPointer(grpc::ServerContext* context,
      const GlVertexAttribPointerRequest* request,
      EmptyResponse* response) override;

 private:
  RenderingUnitService::AsyncService async_;
  ResourcePool* pool_;
};

}  // namespace zen::remote::client::service
