#pragma once

#include <memory>

#include "remote.h"

namespace zen::remote::server {

struct IRenderingUnit {
  virtual ~IRenderingUnit() = default;
};

std::unique_ptr<IRenderingUnit> CreateRenderingUnit(
    std::shared_ptr<IRemote> remote);

}  // namespace zen::remote::server
