#pragma once

#include "client/serial-command-queue.h"
#include "core/common.h"

namespace zen::remote::client::service {
class AsyncSessionKeepaliveCaller;
}

namespace zen::remote::client {

class ResourcePool;

class Session final {
 public:
  DISABLE_MOVE_AND_COPY(Session);
  Session();
  ~Session();

  void Shutdown();

  bool SetKeepaliveCaller(
      service::AsyncSessionKeepaliveCaller *keepalive_caller);

  bool PushCommand(uint64_t serial, std::unique_ptr<ICommand> command);

  inline uint64_t id();
  inline std::shared_ptr<ResourcePool> pool();

 private:
  const uint64_t id_;
  const std::shared_ptr<ResourcePool> pool_;
  bool active_ = true;
  std::mutex active_mutex_;  // priority 0

  std::unique_ptr<SerialCommandQueue> command_queue_;
  std::mutex command_queue_mutex_;  // priority 1

  service::AsyncSessionKeepaliveCaller *keepalive_caller_ = nullptr;
  std::mutex keepalive_caller_mutex_;  // priority 2

  static uint64_t next_id_;
};

inline uint64_t
Session::id()
{
  return id_;
}

inline std::shared_ptr<ResourcePool>
Session::pool()
{
  std::lock_guard<std::mutex> lock(active_mutex_);
  if (active_)
    return pool_;
  else
    return std::shared_ptr<ResourcePool>();
}

}  // namespace zen::remote::client
