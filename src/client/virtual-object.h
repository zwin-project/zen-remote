#pragma once

#include "client/resource.h"
#include "core/common.h"
#include "zen-remote/client/virtual-object.h"

namespace zen::remote::client {

class RenderingUnit;
class AtomicCommandQueue;

class VirtualObject final : public IVirtualObject, public IResource {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(uint64_t id, AtomicCommandQueue *update_rendering_queue);

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void AddRenderingUnit(std::weak_ptr<RenderingUnit> rendering_unit);

  /** Used in the rendering thread */
  void ForEachRenderingUnit(
      std::function<void(IRenderingUnit *)> func) override;

  uint64_t id() override;

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    std::list<std::weak_ptr<RenderingUnit>> rendering_units_;
  } pending_;

  struct {
    std::list<std::weak_ptr<RenderingUnit>> rendering_units_;
  } rendering_;
};

}  // namespace zen::remote::client
