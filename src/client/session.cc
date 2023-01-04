#include "session.h"

#include "client/resource-pool.h"
#include "client/service/async-session-keepalive-caller.h"
#include "core/logger.h"

namespace zen::remote::client {

/** session id 0 is reserved */
uint64_t Session::next_id_ = 1;

Session::Session() : id_(next_id_++), pool_(std::make_shared<ResourcePool>())
{
  LOG_DEBUG("Session %ld started", id_);
}

Session::~Session() { LOG_DEBUG("Session %ld destroyed", id_); }

void
Session::Shutdown()
{
  std::lock_guard<std::mutex> lock_active(active_mutex_);
  std::lock_guard<std::mutex> lock_keepalive(keepalive_caller_mutex_);

  active_ = false;

  if (keepalive_caller_) {
    keepalive_caller_->Finish();
    keepalive_caller_ = nullptr;
  }
}

bool
Session::SetKeepaliveCaller(
    service::AsyncSessionKeepaliveCaller *keepalive_caller)
{
  std::lock_guard<std::mutex> lock_active(active_mutex_);
  std::lock_guard<std::mutex> lock_keepalive(keepalive_caller_mutex_);

  if (!active_ || keepalive_caller_) return false;

  keepalive_caller_ = keepalive_caller;

  return true;
}

void
Session::PushCommand(
    uint64_t serial, uint64_t channel_id, std::unique_ptr<ICommand> command)
{
  std::lock_guard<std::mutex> lock(command_queue_mutex_);

  auto new_queue = std::make_unique<SerialCommandQueue>();

  auto [iter, _] =
      command_queues_.try_emplace(channel_id, std::move(new_queue));

  iter->second->Push(serial, std::move(command));

  if (command_queues_.size() > 100) {
    for (auto it = command_queues_.begin(); it != command_queues_.end();) {
      if (it->second->IsEmpty()) {
        it = command_queues_.erase(it);
      } else {
        it++;
      }
    }
  }
}

}  // namespace zen::remote::client
