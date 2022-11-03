#include "client/atomic-command-queue.h"

namespace zen::remote::client {

AtomicCommandQueue::~AtomicCommandQueue()
{
  while (!queue_.empty()) {
    if (queue_.front().is_commit_command) {
      queue_.pop_front();
    } else {
      auto &command = queue_.front().command;

      command->Cancel();

      queue_.pop_front();
    }
  }
}

void
AtomicCommandQueue::Push(std::unique_ptr<ICommand> command)
{
  std::lock_guard<std::mutex> lock(mtx_);
  queue_.emplace_back(std::move(command), false);
}

void
AtomicCommandQueue::Commit()
{
  std::lock_guard<std::mutex> lock(mtx_);
  commit_count_++;
  queue_.emplace_back(std::unique_ptr<ICommand>(), true);
}

bool
AtomicCommandQueue::ExecuteOnce()
{
  std::lock_guard<std::mutex> exec_lock(exec_mtx_);

  if (commit_count() == 0) {
    return false;
  }

  std::unique_lock<std::mutex> lock(mtx_);

  while (true) {
    if (queue_.front().is_commit_command) {
      commit_count_--;
      queue_.pop_front();
      return true;
    }

    auto &command = queue_.front().command;

    lock.unlock();

    command->Execute();

    lock.lock();

    queue_.pop_front();
  }
}

size_t
AtomicCommandQueue::commit_count()
{
  std::lock_guard<std::mutex> lock(mtx_);
  return commit_count_;
}

AtomicCommandQueue::CommandInfo::CommandInfo(
    std::unique_ptr<ICommand> command, bool is_commit_command)
    : command(std::move(command)), is_commit_command(is_commit_command)
{
}

}  // namespace zen::remote::client
