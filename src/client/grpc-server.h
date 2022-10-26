#pragma once

#include "core/common.h"

namespace zen::remote::client {

class ResourcePool;

class GrpcServer {
 public:
  DISABLE_MOVE_AND_COPY(GrpcServer);
  GrpcServer() = delete;
  GrpcServer(std::string host, uint16_t port, ResourcePool *pool);

  ~GrpcServer();

  void Start();

 private:
  const std::string host_;
  const uint16_t port_;
  std::thread thread_;
  std::unique_ptr<grpc::Server> server_;
  std::unique_ptr<grpc::ServerCompletionQueue> completion_queue_;

  ResourcePool *pool_;
};

}  // namespace zen::remote::client
