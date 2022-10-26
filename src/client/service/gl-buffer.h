#pragma once

#include "client/service/serial-async-service.h"
#include "core/common.h"
#include "gl-buffer.grpc.pb.h"

namespace zen::remote::client {
class ResourcePool;
}

namespace zen::remote::client::service {

class GlBufferServiceImpl final : public GlBufferService::Service,
                                  public ISerialAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(GlBufferServiceImpl);
  GlBufferServiceImpl() = delete;
  GlBufferServiceImpl(ResourcePool* pool);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue,
      SerialCommandQueue* command_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status GlBufferData(grpc::ServerContext* context,
      const GlBufferDataRequest* request, EmptyResponse* response) override;

 private:
  GlBufferService::AsyncService async_;
  ResourcePool* pool_;
};

}  // namespace zen::remote::client::service
