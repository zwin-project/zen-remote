#include "client/resource-pool.h"

#include "core/logger.h"

namespace zen::remote::client {

void
ResourcePool::ForEachRenderingUnit(
    std::function<void(std::shared_ptr<IRenderingUnit>&)> func)
{
  rendering_units_->ForEach([func](const std::shared_ptr<RenderingUnit>& unit) {
    auto u = std::dynamic_pointer_cast<IRenderingUnit>(unit);
    func(u);
  });
}

}  // namespace zen::remote::client
