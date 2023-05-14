#pragma once

#include <zen-remote/server/channel.h>

#include <memory>

namespace zen::remote::server {

struct IVirtualObject {
  virtual ~IVirtualObject() = default;

  virtual void Commit() = 0;

  virtual void Move(float position[3], float quaternion[4]) = 0;

  virtual void ChangeVisibility(bool visible) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IVirtualObject> CreateVirtualObject(
    std::shared_ptr<IChannel> channel);

}  // namespace zen::remote::server
