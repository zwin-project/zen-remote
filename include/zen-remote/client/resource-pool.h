#pragma once

#include <zen-remote/client/rendering-unit.h>

#include <functional>
#include <memory>

namespace zen::remote::client {

struct IResourcePool {
  virtual ~IResourcePool() = default;

  virtual void ForEachRenderingUnit(
      std::function<void(IRenderingUnit*)> func) = 0;
};

}  // namespace zen::remote::client
