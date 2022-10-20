#pragma once

#include "client/resource.h"
#include "core/common.h"
#include "zen-remote/client/virtual-object.h"

namespace zen::remote::client {

class RenderingUnit;

class VirtualObject final : public IVirtualObject, public IResource {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(uint64_t id);

  uint64_t id() override;

  void ForEachRenderingUnit(std::function<void(IRenderingUnit*)> func) override;

  void AddRenderingUnit(std::weak_ptr<RenderingUnit> rendering_unit);

 private:
  const uint64_t id_;
};

}  // namespace zen::remote::client
