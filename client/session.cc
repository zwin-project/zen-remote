#include "session.h"

#include "logger.h"

namespace zen::display_system::remote::client {

bool
Session::Init()
{
  LOG_INFO("Remote Display System Session Client Initialized");
  return true;
}

std::unique_ptr<ISession>
SessionCreate()
{
  return std::make_unique<Session>();
}

}  // namespace zen::display_system::remote::client
