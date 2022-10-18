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
  ResourcePool() : rendering_units_(std::make_unique<RenderingUnitContainer>())
  {
  }

  void ForEachRenderingUnit(
      std::function<void(std::shared_ptr<IRenderingUnit> &)> func) override;

  inline std::unique_ptr<RenderingUnitContainer> &rendering_units();

 private:
  std::unique_ptr<RenderingUnitContainer> rendering_units_;
};

inline std::unique_ptr<RenderingUnitContainer> &
ResourcePool::rendering_units()
{
  return rendering_units_;
}

}  // namespace zen::remote::client
