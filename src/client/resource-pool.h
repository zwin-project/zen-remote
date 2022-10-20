#pragma once

#include "client/gl-buffer.h"
#include "client/rendering-unit.h"
#include "client/resource-container.h"
#include "client/virtual-object.h"
#include "core/common.h"
#include "zen-remote/client/resource-pool.h"

namespace zen::remote::client {

using VirtualObjectContainer =
    ResourceContainer<VirtualObject, ResourceContainerType::kLoopIntensive>;

using RenderingUnitContainer =
    ResourceContainer<RenderingUnit, ResourceContainerType::kFindByIdIntensive>;

using GlBufferContainer =
    ResourceContainer<GlBuffer, ResourceContainerType::kFindByIdIntensive>;

class ResourcePool final : public IResourcePool {
 public:
  DISABLE_MOVE_AND_COPY(ResourcePool);
  ResourcePool() = default;

  void Traverse(std::function<void(IVirtualObject *)> func) override;

  inline VirtualObjectContainer *virtual_objects();
  inline RenderingUnitContainer *rendering_units();
  inline GlBufferContainer *gl_buffers();

 private:
  VirtualObjectContainer virtual_objects_;
  RenderingUnitContainer rendering_units_;
  GlBufferContainer gl_buffers_;
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

}  // namespace zen::remote::client
