#pragma once

#include "client/command.h"
#include "core/common.h"

namespace zen::remote::client {

/**
 * Push multiple commands and commit them. This handles those commands
 * atomically. The pushed commands will not be executed until they are
 * committed. All methods can be called from multiple threads.
 */
class AtomicCommandQueue {
 public:
  DISABLE_MOVE_AND_COPY(AtomicCommandQueue);
  AtomicCommandQueue() = default;
  ~AtomicCommandQueue();

  void Push(std::unique_ptr<ICommand> command);

  void Commit();

  /**
   * Execute one set of commands.
   */
  bool ExecuteOnce();

  size_t commit_count();

 private:
  struct CommandInfo {
    CommandInfo(std::unique_ptr<ICommand> command, bool is_commit_command);

    std::unique_ptr<ICommand> command;  // empty if is_commit_command == true
    bool is_commit_command;
  };

  std::list<CommandInfo> queue_;
  size_t commit_count_;
  std::mutex mtx_;  // lock queue_ and commit_count_;

  std::mutex exec_mtx_;
};

}  // namespace zen::remote::client
