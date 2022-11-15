#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "gl-shader.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}  // namespace zen::remote::client

namespace zen::remote::client::service {

class GlShaderServiceImpl final : public GlShaderService::Service,
                                  public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(GlShaderServiceImpl);
  GlShaderServiceImpl() = delete;
  GlShaderServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewGlShaderRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

 private:
  GlShaderService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
