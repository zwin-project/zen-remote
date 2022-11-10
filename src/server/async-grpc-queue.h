#pragma once

#include "core/common.h"

namespace zen::remote::server {

struct AsyncGrpcCallerBase;

class AsyncGrpcQueue {
 public:
  DISABLE_MOVE_AND_COPY(AsyncGrpcQueue);
  AsyncGrpcQueue();
  ~AsyncGrpcQueue();

  void Push(std::unique_ptr<AsyncGrpcCallerBase> caller);

  void Start();

  void Terminate();

 private:
  std::thread thread_;
  std::shared_ptr<grpc::CompletionQueue> cq_;
};

}  // namespace zen::remote::server
