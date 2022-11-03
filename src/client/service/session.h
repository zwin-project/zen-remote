#pragma once

#include "client/service/async-service.h"
#include "core/common.h"
#include "session.grpc.pb.h"

namespace zen::remote::client {
class Remote;
}

namespace zen::remote::client::service {

class SessionServiceImpl final : public SessionService::Service,
                                 public IAsyncService {
 public:
  DISABLE_MOVE_AND_COPY(SessionServiceImpl);
  SessionServiceImpl() = delete;
  SessionServiceImpl(Remote* remote);

  void Register(grpc::ServerBuilder& builder) override;

  void Listen(grpc::ServerCompletionQueue* completion_queue) override;

  grpc::Status New(grpc::ServerContext* context,
      const NewSessionRequest* request, NewSessionResponse* response) override;

 private:
  SessionService::AsyncService async_;
  Remote* remote_;
};

}  // namespace zen::remote::client::service
