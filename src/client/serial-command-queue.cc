#include "client/serial-command-queue.h"

namespace zen::remote::client {

void
SerialCommandQueue::Push(uint64_t serial, std::unique_ptr<ICommand> command)
{
  if (serial < next_serial_) {
    command->Execute();

  } else if (serial == next_serial_) {
    command->Execute();
    next_serial_++;
    Consume();
  } else {
    Insert(serial, std::move(command));
  }
}

void
SerialCommandQueue::Consume()
{
  while (!list_.empty()) {
    auto& [serial, command] = list_.front();
    if (serial != next_serial_) break;

    command->Execute();
    next_serial_++;
    list_.pop_front();
  }
}

void
SerialCommandQueue::Insert(uint64_t serial, std::unique_ptr<ICommand> command)
{
  auto it = list_.end();

  while (it != list_.begin() && (*std::prev(it)).first > serial) {
    it--;
  }

  list_.emplace(it, serial, std::move(command));
}

}  // namespace zen::remote::client
