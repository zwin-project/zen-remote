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

  inline uint32_t pending_count();

 private:
  std::thread thread_;
  std::shared_ptr<grpc::CompletionQueue> cq_;
  std::atomic_int32_t pending_count_ = 0;

  bool enabled_ = false;
  std::mutex mtx_;
};

inline uint32_t
AsyncGrpcQueue::pending_count()
{
  return pending_count_.load();
}

}  // namespace zen::remote::server
