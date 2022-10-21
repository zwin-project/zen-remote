#include "client/command.h"

namespace zen::remote::client {

Command::Command(std::function<void()> executer) : executer_(executer) {}

void
Command::Execute()
{
  executer_();
}

}  // namespace zen::remote::client
