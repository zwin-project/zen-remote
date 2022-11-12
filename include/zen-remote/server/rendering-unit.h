#pragma once

#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IRenderingUnit {
  virtual ~IRenderingUnit() = default;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IRenderingUnit> CreateRenderingUnit(
    std::shared_ptr<ISession> session, uint64_t virtual_object_id);

}  // namespace zen::remote::server
