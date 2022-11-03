#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "gl-buffer.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}

namespace zen::remote::client::service {

class GlBufferServiceImpl final : public GlBufferService::Service,
                                  public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(GlBufferServiceImpl);
  GlBufferServiceImpl() = delete;
  GlBufferServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status GlBufferData(grpc::ServerContext* context,
      const GlBufferDataRequest* request, EmptyResponse* response) override;

 private:
  GlBufferService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
