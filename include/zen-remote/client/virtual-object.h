#pragma once

#include <zen-remote/client/rendering-unit.h>

#include <functional>

namespace zen::remote::client {

struct IVirtualObject {
  virtual ~IVirtualObject() = default;

  virtual void ForEachRenderingUnit(
      std::function<void(IRenderingUnit*)> func) = 0;
};

}  // namespace zen::remote::client
