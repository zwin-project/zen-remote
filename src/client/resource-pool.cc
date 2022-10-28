#include "client/resource-pool.h"

#include "core/logger.h"

namespace zen::remote::client {

void
ResourcePool::UpdateRenderingState()
{
  auto execute_count = update_rendering_queue_.commit_count();

  for (uint i = 0; i < execute_count; i++) {
    if (update_rendering_queue_.ExecuteOnce() == false) break;
  }
}

}  // namespace zen::remote::client
