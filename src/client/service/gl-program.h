#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "gl-program.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}

namespace zen::remote::client::service {

class GlProgramServiceImpl final : public GlProgramService::Service,
                                   public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(GlProgramServiceImpl);
  GlProgramServiceImpl() = delete;
  GlProgramServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status GlAttachShader(grpc::ServerContext* context,
      const GlAttachShaderRequest* request, EmptyResponse* response) override;

  grpc::Status GlLinkProgram(::grpc::ServerContext* context,
      const GlLinkProgramRequest* request, EmptyResponse* response) override;

 private:
  GlProgramService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
