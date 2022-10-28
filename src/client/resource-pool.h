#pragma once

#include "client/atomic-command-queue.h"
#include "client/gl-buffer.h"
#include "client/rendering-unit.h"
#include "client/resource-container.h"
#include "client/virtual-object.h"
#include "core/common.h"

namespace zen::remote::client {

using VirtualObjectContainer =
    ResourceContainer<VirtualObject, ResourceContainerType::kLoopIntensive>;

using RenderingUnitContainer =
    ResourceContainer<RenderingUnit, ResourceContainerType::kFindByIdIntensive>;

using GlBufferContainer =
    ResourceContainer<GlBuffer, ResourceContainerType::kFindByIdIntensive>;

/**
 * @brief Retain all resources
 *
 * Resources are updated by the "update thread" and the updated states are
 * temporarily held in the "pending state" of the respective resource.
 * When a set of resources are commited, commands to update the "rendering
 * state" based on the "pending state" are pushed to `update_rendering_queue`.
 * The pushed commands are executed by the "rendering thread" just before the
 * resource pool is traversed to render scene. At this time, commands issued in
 * a single commit are executed atomically, thus maintaining the integrity of
 * the entire "rendering state".
 * These commands should not read the pending state, since the pending state may
 * be updated between when these commands are pushed and when they are executed.
 * Instead, use a lambda capture or something similar to preserve the state at
 * the time of commit. This also helps keep multi-threading safe.
 */
class ResourcePool {
 public:
  DISABLE_MOVE_AND_COPY(ResourcePool);
  ResourcePool() = default;

  /** Used in the rendering thread */
  void UpdateRenderingState();

  inline VirtualObjectContainer *virtual_objects();
  inline RenderingUnitContainer *rendering_units();
  inline GlBufferContainer *gl_buffers();

  /** Commands to update rendering state of resources */
  inline AtomicCommandQueue *update_rendering_queue();

 private:
  VirtualObjectContainer virtual_objects_;
  RenderingUnitContainer rendering_units_;
  GlBufferContainer gl_buffers_;

  AtomicCommandQueue update_rendering_queue_;
};

inline VirtualObjectContainer *
ResourcePool::virtual_objects()
{
  return &virtual_objects_;
}

inline RenderingUnitContainer *
ResourcePool::rendering_units()
{
  return &rendering_units_;
}

inline GlBufferContainer *
ResourcePool::gl_buffers()
{
  return &gl_buffers_;
}

inline AtomicCommandQueue *
ResourcePool::update_rendering_queue()
{
  return &update_rendering_queue_;
}

}  // namespace zen::remote::client
