#include "server/job-queue.h"

namespace zen::remote::server {

JobQueue::~JobQueue()
{
  running_ = false;

  if (!thread_.joinable()) return;

  cond_.notify_one();

  thread_.join();
}

void
JobQueue::Push(std::unique_ptr<Job> job)
{
  {
    std::lock_guard<std::mutex> lock(queue_mtx_);
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

}  // namespace zen::remote::server
