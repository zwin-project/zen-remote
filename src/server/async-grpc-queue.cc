#include "async-grpc-queue.h"

#include "async-grpc-caller.h"

namespace zen::remote::server {

AsyncGrpcQueue::~AsyncGrpcQueue() { Terminate(); }

void
AsyncGrpcQueue::Push(std::unique_ptr<AsyncGrpcCallerBase> caller)
{
  auto caller_raw = caller.release();

  caller_raw->Start(&cq_);
}

void
AsyncGrpcQueue::Start()
{
  if (thread_.joinable()) return;

  thread_ = std::thread([this] {
    void *tag;
    bool ok = false;

    while (cq_.Next(&tag, &ok)) {
      auto caller = static_cast<AsyncGrpcCallerBase *>(tag);

      caller->Finish();

      delete caller;
    }
  });
}

void
AsyncGrpcQueue::Terminate()
{
  if (!thread_.joinable()) return;

  cq_.Shutdown();

  thread_.join();
}

}  // namespace zen::remote::server
