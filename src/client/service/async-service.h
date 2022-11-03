#pragma once

namespace zen::remote::client::service {

struct IAsyncService {
  virtual ~IAsyncService() = default;

  virtual void Register(grpc::ServerBuilder& builder) = 0;
  virtual void Listen(grpc::ServerCompletionQueue* completion_queue) = 0;
};

}  // namespace zen::remote::client::service
