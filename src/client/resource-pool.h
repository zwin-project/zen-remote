#pragma once

#include "client/gl-buffer.h"
#include "client/rendering-unit.h"
#include "client/resource-container.h"
#include "core/common.h"
#include "zen-remote/client/resource-pool.h"

namespace zen::remote::client {

using RenderingUnitContainer =
    ResourceContainer<RenderingUnit, ResourceContainerType::kLoopIntensive>;

using GlBufferContainer =
    ResourceContainer<GlBuffer, ResourceContainerType::kFindByIdIntensive>;

class ResourcePool final : public IResourcePool {
 public:
  DISABLE_MOVE_AND_COPY(ResourcePool);
  ResourcePool() = default;

  void ForEachRenderingUnit(
      std::function<void(IRenderingUnit *)> func) override;

  inline RenderingUnitContainer *rendering_units();
  inline GlBufferContainer *gl_buffers();

 private:
  RenderingUnitContainer rendering_units_;
  GlBufferContainer gl_buffers_;
};

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
