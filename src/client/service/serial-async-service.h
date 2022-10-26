#pragma once

namespace zen::remote::client {

class SerialCommandQueue;

}  // namespace zen::remote::client

namespace zen::remote::client::service {

struct ISerialAsyncService {
  virtual ~ISerialAsyncService() = default;

  virtual void Register(grpc::ServerBuilder& builder) = 0;
  virtual void Listen(grpc::ServerCompletionQueue* completion_queue,
      SerialCommandQueue* command_queue) = 0;
};

}  // namespace zen::remote::client::service
