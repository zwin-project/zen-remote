#include "session.h"

#include "client/resource-pool.h"
#include "core/logger.h"

namespace zen::remote::client {

/** session id 0 is reserved */
uint64_t Session::next_id_ = 1;

Session::Session() : id_(next_id_++), pool_(std::make_shared<ResourcePool>())
{
  LOG_DEBUG("Session %ld started", id_);
}

Session::~Session() { LOG_DEBUG("Session %ld destroyed", id_); }

}  // namespace zen::remote::client
