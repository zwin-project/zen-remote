#pragma once

#include "client/serial-command-queue.h"
#include "core/common.h"
#include "zen-remote/client/session.h"

namespace zen::remote::client {

class ResourcePool;

class Session final : public ISession {
 public:
  DISABLE_MOVE_AND_COPY(Session);
  Session();
  ~Session();

  inline uint64_t id();
  inline std::shared_ptr<ResourcePool> pool();
  inline SerialCommandQueue* command_queue();

 private:
  const uint64_t id_;
  const std::shared_ptr<ResourcePool> pool_;
  SerialCommandQueue command_queue_;

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
  return pool_;
}

inline SerialCommandQueue*
Session::command_queue()
{
  return &command_queue_;
}

}  // namespace zen::remote::client
