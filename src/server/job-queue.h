#pragma once

#include "core/common.h"
#include "server/job.h"

namespace zen::remote::server {

/**
 * JobQueue manages jobs and the worker thread that executes them.
 * Once "StartWorkerThread" is called, the worker thread continues to run until
 * the JobQueue is destroyed; when the JobQueue is destroyed, any jobs remaining
 * in the queue are canceled.
 */
class JobQueue {
 public:
  DISABLE_MOVE_AND_COPY(JobQueue);
  JobQueue() = default;
  ~JobQueue();

  void Push(std::unique_ptr<IJob> job);

  void StartWorkerThread();

  void Terminate();

 private:
  std::queue<std::unique_ptr<IJob>> queue_;
  std::mutex queue_mtx_;

  std::condition_variable cond_;

  std::atomic_bool running_;
  std::thread thread_;
};

}  // namespace zen::remote::server
