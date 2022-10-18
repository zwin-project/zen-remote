#pragma once

#include <zen-remote/client/rendering-unit.h>

#include <functional>
#include <memory>

namespace zen::remote::client {

struct IResourcePool {
  ~IResourcePool() = default;

  virtual void ForEachRenderingUnit(
      std::function<void(std::shared_ptr<IRenderingUnit>&)> func) = 0;
};

}  // namespace zen::remote::client
