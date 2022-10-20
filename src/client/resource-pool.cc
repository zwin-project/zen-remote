#include "client/resource-pool.h"

#include "core/logger.h"

namespace zen::remote::client {

void
ResourcePool::Traverse(std::function<void(IVirtualObject *)> func)
{
  virtual_objects_.ForEach(
      [func](const std::shared_ptr<VirtualObject> virtual_object) {
        func(virtual_object.get());
      });
}
}  // namespace zen::remote::client
