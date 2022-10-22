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
struct IJob {
  virtual ~IJob() = default;
  virtual void Perform(bool cancel) = 0;
};

/**
 * This accepts non-copyable perform_func
 */
template <typename F>
class Job final : public IJob {
 public:
  DISABLE_MOVE_AND_COPY(Job);
  Job() = delete;
  Job(F&& perform_func) : perform_func_(std::forward<F>(perform_func)) {}

  void Perform(bool cancel) override { perform_func_(cancel); }

  F perform_func_;
};

template <typename F>
std::unique_ptr<IJob>
CreateJob(F&& perform_func)
{
  return std::unique_ptr<IJob>(new Job<F>(std::forward<F>(perform_func)));
}

}  // namespace zen::remote::server
