#pragma once

#include <zen-remote/client/virtual-object.h>

#include <functional>

namespace zen::remote::client {

struct IResourcePool {
  virtual ~IResourcePool() = default;

  virtual void Traverse(std::function<void(IVirtualObject*)> func) = 0;
};

}  // namespace zen::remote::client
