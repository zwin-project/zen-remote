#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "gl-texture.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}  // namespace zen::remote::client

namespace zen::remote::client::service {

class GlTextureServiceImpl final : public GlTextureService::Service,
                                   public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(GlTextureServiceImpl);
  GlTextureServiceImpl() = delete;
  GlTextureServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status GlTexImage2D(grpc::ServerContext* context,
      const GlTexImage2DRequest* request, EmptyResponse* response) override;

 private:
  GlTextureService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
