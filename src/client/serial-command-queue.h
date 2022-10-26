#pragma once

#include "client/command.h"
#include "core/common.h"

namespace zen::remote::client {

/**
 * Push commands with their serial number. The commands are executed in the
 * order of their serial numbers, starting with number 0. For example, if
 * commands are pushed in the order C0, C1, C4, C3, C6, C2, and C5 (where Cx is
 * the command with serial number x), C2, C3, and C4 are executed when C2 is
 * pushed, and C5 and C6 are executed when C5 is pushed.
 * Commands are expected to be pushed in serial number order for the most of the
 * time.
 * Multi-threading is not supported.
 */
class SerialCommandQueue {
 public:
  DISABLE_MOVE_AND_COPY(SerialCommandQueue);
  SerialCommandQueue() = default;

  void Push(uint64_t serial, std::unique_ptr<ICommand> command);

 private:
  void Consume();
  void Insert(uint64_t serial, std::unique_ptr<ICommand> command);

  uint64_t next_serial_ = 0;
  std::list<std::pair<uint64_t, std::unique_ptr<ICommand>>> list_;
};

}  // namespace zen::remote::client
