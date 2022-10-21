#include "client/resource-pool.h"

#include "core/logger.h"

namespace zen::remote::client {

void
ResourcePool::Traverse(std::function<void(IVirtualObject *)> func)
{
  auto execute_count = update_rendering_queue_.commit_count();

  for (unsigned int i = 0; i < execute_count; i++) {
    if (update_rendering_queue_.ExecuteOnce() == false) break;
  }

  virtual_objects_.ForEach(
      [func](const std::shared_ptr<VirtualObject> virtual_object) {
        func(virtual_object.get());
      });
}
}  // namespace zen::remote::client
