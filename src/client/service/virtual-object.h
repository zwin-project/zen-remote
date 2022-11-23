#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "virtual-object.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}

namespace zen::remote::client::service {

class VirtualObjectServiceImpl final : public VirtualObjectService::Service,
                                       public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObjectServiceImpl);
  VirtualObjectServiceImpl() = delete;
  VirtualObjectServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewResourceRequest* request, EmptyResponse* response) override;

  grpc::Status Delete(grpc::ServerContext* context,
      const DeleteResourceRequest* request, EmptyResponse* response) override;

  grpc::Status Commit(grpc::ServerContext* context,
      const VirtualObjectCommitRequest* request,
      EmptyResponse* response) override;

  grpc::Status Move(grpc::ServerContext* context,
      const VirtualObjectMoveRequest* request,
      EmptyResponse* response) override;

 private:
  VirtualObjectService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
