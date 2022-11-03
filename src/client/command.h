#pragma once

#include "core/common.h"

namespace zen::remote::client {

struct ICommand {
  virtual ~ICommand() = default;
  virtual void Execute() = 0;
  virtual void Cancel() = 0;
};

/**
 * This accepts non-copyable executor
 */
template <typename F>
class Command final : public ICommand {
 public:
  DISABLE_MOVE_AND_COPY(Command);
  Command() = delete;
  Command(F&& executer) : executer_(std::forward<F>(executer)){};

  void Execute() override { executer_(false); }

  void Cancel() override { executer_(true); }

 private:
  F executer_;
};

template <typename F>
std::unique_ptr<ICommand>
CreateCommand(F&& executor)
{
  return std::unique_ptr<ICommand>(new Command<F>(std::forward<F>(executor)));
}

}  // namespace zen::remote::client
