#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "peer.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}

namespace zen::remote::client::service {

class PeerServiceImpl final : public PeerService::Service,
                              public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(PeerServiceImpl);
  PeerServiceImpl() = delete;
  PeerServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status Probe(grpc::ServerContext* context,
      const PeerProbeRequest* request, EmptyResponse* response) override;

 private:
  PeerService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
