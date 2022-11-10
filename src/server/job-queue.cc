#include "server/job-queue.h"

namespace zen::remote::server {

JobQueue::~JobQueue() { Terminate(); }

void
JobQueue::Push(std::unique_ptr<IJob> job)
{
  {
    std::unique_lock<std::mutex> lock(queue_mtx_);

    if (running_ == false) {
      lock.unlock();
      job->Perform(true);
      return;
    }

    queue_.push(std::move(job));
  }

  cond_.notify_one();
}

void
JobQueue::StartWorkerThread()
{
  if (thread_.joinable()) return;

  running_ = true;

  thread_ = std::thread([this] {
    std::unique_lock<std::mutex> lock(queue_mtx_);
    for (;;) {
      cond_.wait(lock, [this] { return !queue_.empty() || !running_; });

      if (!running_) break;

      auto &job = queue_.front();

      lock.unlock();

      job->Perform(false);

      lock.lock();

      queue_.pop();
    }

    while (!queue_.empty()) {
      auto &job = queue_.front();
      job->Perform(true);
      queue_.pop();
    }
  });
}

void
JobQueue::Terminate()
{
  {
    std::lock_guard<std::mutex> lock(queue_mtx_);
    if (running_ == false) return;

    running_ = false;
  }

  if (!thread_.joinable()) return;

  cond_.notify_one();

  thread_.join();
}

}  // namespace zen::remote::server
