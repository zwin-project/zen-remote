#include "session-serial.h"

namespace zen::remote::server {

uint64_t
SessionSerial::NextResourceSerial()
{
  std::lock_guard<std::mutex> lock(resource_.mtx);
  return resource_.serial++;
}

uint64_t
SessionSerial::NextChannelSerial()
{
  std::lock_guard<std::mutex> lock(channel_.mtx);
  return channel_.serial++;
}

}  // namespace zen::remote::server
