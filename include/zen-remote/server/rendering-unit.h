#pragma once

#include <zen-remote/server/channel.h>

#include <memory>

namespace zen::remote::server {

struct IRenderingUnit {
  virtual ~IRenderingUnit() = default;

  virtual uint64_t id() = 0;

  virtual void ChangeVisibility(bool visible) = 0;
};

std::unique_ptr<IRenderingUnit> CreateRenderingUnit(
    std::shared_ptr<IChannel> channel, uint64_t virtual_object_id);

}  // namespace zen::remote::server
