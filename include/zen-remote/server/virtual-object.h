#pragma once

#include <zen-remote/server/remote.h>

#include <memory>

namespace zen::remote::server {

struct IVirtualObject {
  virtual ~IVirtualObject() = default;

  virtual void Commit() = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IVirtualObject> CreateVirtualObject(
    std::shared_ptr<IRemote> remote);

}  // namespace zen::remote::server
