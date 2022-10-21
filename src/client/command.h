#pragma once

#include "core/common.h"

namespace zen::remote::client {

class Command {
 public:
  DISABLE_MOVE_AND_COPY(Command);
  Command(std::function<void()> executer);

  void Execute();

 private:
  const std::function<void()> executer_;
};

}  // namespace zen::remote::client
