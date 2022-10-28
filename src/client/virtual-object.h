#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

struct Camera;
class RenderingUnit;
class AtomicCommandQueue;

class VirtualObject final : public IResource {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(uint64_t id, AtomicCommandQueue *update_rendering_queue);

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void AddRenderingUnit(std::weak_ptr<RenderingUnit> rendering_unit);

  /** Used in the rendering thread */
  void Render(Camera *camera);

  uint64_t id() override;

  /** Used in the rendering thread */
  inline bool commited();

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    std::list<std::weak_ptr<RenderingUnit>> rendering_units_;
  } pending_;

  struct {
    bool commited = false;
    std::list<std::weak_ptr<RenderingUnit>> rendering_units_;
  } rendering_;
};

inline bool
VirtualObject::commited()
{
  return rendering_.commited;
}

}  // namespace zen::remote::client
