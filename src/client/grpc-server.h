#pragma once

#include "client/resource-pool.h"
#include "core/common.h"

namespace zen::remote::client {

class GrpcServer {
 public:
  DISABLE_MOVE_AND_COPY(GrpcServer);
  GrpcServer() = delete;
  GrpcServer(
      std::string host, uint16_t port, std::shared_ptr<ResourcePool> pool)
      : host_(host), port_(port), pool_(std::move(pool))
  {
  }
  ~GrpcServer();

  void Start();

 private:
  const std::string host_;
  const uint16_t port_;
  std::thread thread_;
  std::unique_ptr<grpc::Server> server_;
  std::shared_ptr<ResourcePool> pool_;
};

}  // namespace zen::remote::client
