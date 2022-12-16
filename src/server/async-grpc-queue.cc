#include "async-grpc-queue.h"

#include "async-grpc-caller.h"

namespace zen::remote::server {

AsyncGrpcQueue::~AsyncGrpcQueue() { Terminate(); }

AsyncGrpcQueue::AsyncGrpcQueue()
    : cq_(std::make_shared<grpc::CompletionQueue>())
{
}

void
AsyncGrpcQueue::Push(std::unique_ptr<AsyncGrpcCallerBase> caller)
{
  auto caller_raw = caller.release();

  caller_raw->Start(cq_.get());

  pending_count_++;
}

void
AsyncGrpcQueue::Start()
{
  if (thread_.joinable()) return;

  thread_ = std::thread([cq = cq_, this] {
    void *tag;
    bool ok = false;

    while (cq->Next(&tag, &ok)) {
      auto caller = static_cast<AsyncGrpcCallerBase *>(tag);

      caller->Finish();

      delete caller;

      pending_count_--;
    }
  });
}

void
AsyncGrpcQueue::Terminate()
{
  if (!thread_.joinable()) return;

  cq_->Shutdown();

  thread_.detach();
}

}  // namespace zen::remote::server
