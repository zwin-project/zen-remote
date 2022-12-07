#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "gl-sampler.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}  // namespace zen::remote::client

namespace zen::remote::client::service {

class GlSamplerServiceImpl final : public GlSamplerService::Service,
                                   public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(GlSamplerServiceImpl);
  GlSamplerServiceImpl() = delete;
  GlSamplerServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status GlSamplerParameterf(grpc::ServerContext* context,
      const GlSamplerParameterfRequest* request,
      EmptyResponse* response) override;

  grpc::Status GlSamplerParameteri(grpc::ServerContext* context,
      const GlSamplerParameteriRequest* request,
      EmptyResponse* response) override;

  grpc::Status GlSamplerParameterfv(grpc::ServerContext* context,
      const GlSamplerParametervRequest* request,
      EmptyResponse* response) override;

  grpc::Status GlSamplerParameteriv(grpc::ServerContext* context,
      const GlSamplerParametervRequest* request,
      EmptyResponse* response) override;

  grpc::Status GlSamplerParameterIiv(grpc::ServerContext* context,
      const GlSamplerParametervRequest* request,
      EmptyResponse* response) override;

  grpc::Status GlSamplerParameterIuiv(grpc::ServerContext* context,
      const GlSamplerParametervRequest* request,
      EmptyResponse* response) override;

 private:
  GlSamplerService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
