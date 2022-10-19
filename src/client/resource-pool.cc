#include "client/resource-pool.h"

#include "core/logger.h"

namespace zen::remote::client {

void
ResourcePool::ForEachRenderingUnit(std::function<void(IRenderingUnit*)> func)
{
  rendering_units_.ForEach(
      [func](const std::shared_ptr<RenderingUnit> unit) { func(unit.get()); });
}

}  // namespace zen::remote::client
