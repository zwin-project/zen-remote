#pragma once

#include "core/common.h"

namespace zen::remote::server {

/**
 * Jobs pushed in a JobQueue are performed in a worker thread controlled by the
 * JobQueue. Jobs are performed in the order they are pushed.
 * A job can be cancelled, ant then "perform_func" is called with
 * the cancel argument 'true'. In this case, "perform_func" should return as
 * quickly as possible.
 */
class Job {
 public:
  DISABLE_MOVE_AND_COPY(Job);
  Job() = delete;
  Job(std::function<void(bool cancel)> perform_func);

 private:
  friend class JobQueue;
  void Perform(bool cancel);

  std::function<void(bool cancel)> perform_func_;
};

}  // namespace zen::remote::server
