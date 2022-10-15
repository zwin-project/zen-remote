#pragma once

#include "core/common.h"

namespace zen::remote::client {

class GrpcServer {
 public:
  DISABLE_MOVE_AND_COPY(GrpcServer);
  GrpcServer(std::string host, uint16_t port) : host_(host), port_(port) {}
  ~GrpcServer();

  void Start();

 private:
  const std::string host_;
  const uint16_t port_;
  std::thread thread_;
  std::unique_ptr<grpc::Server> server_;
};

}  // namespace zen::remote::client
