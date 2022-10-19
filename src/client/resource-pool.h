#pragma once

#include "client/rendering-unit.h"
#include "client/resource-container.h"
#include "core/common.h"
#include "zen-remote/client/resource-pool.h"

namespace zen::remote::client {

using RenderingUnitContainer =
    ResourceContainer<RenderingUnit, ResourceContainerType::kLoopIntensive>;

class ResourcePool final : public IResourcePool {
 public:
  DISABLE_MOVE_AND_COPY(ResourcePool);
  ResourcePool() = default;

  void ForEachRenderingUnit(
      std::function<void(IRenderingUnit *)> func) override;

  inline RenderingUnitContainer *rendering_units();

 private:
  RenderingUnitContainer rendering_units_;
};

inline RenderingUnitContainer *
ResourcePool::rendering_units()
{
  return &rendering_units_;
}

}  // namespace zen::remote::client
