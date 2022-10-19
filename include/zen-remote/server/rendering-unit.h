#pragma once

#include <zen-remote/server/remote.h>

#include <memory>

namespace zen::remote::server {

struct IRenderingUnit {
  virtual ~IRenderingUnit() = default;
};

std::unique_ptr<IRenderingUnit> CreateRenderingUnit(
    std::shared_ptr<IRemote> remote);

}  // namespace zen::remote::server
