#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

struct Camera;
class RenderingUnit;
class AtomicCommandQueue;

class VirtualObject final : public IResource {
  struct RenderingState {
    bool commited = false;
    std::list<std::weak_ptr<RenderingUnit>> rendering_units_;
    glm::vec3 position = {0, 0, 0};
    glm::quat quaternion = {1, 0, 0, 0};
    bool visible = false;
  };

 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(uint64_t id, AtomicCommandQueue *update_rendering_queue);
  ~VirtualObject();

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void Move(glm::vec3 position, glm::quat quaternion);

  /** Used in the update thread */
  void ChangeVisibility(bool visible);

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

  std::shared_ptr<RenderingState> rendering_;  // nonnull
};

inline bool
VirtualObject::commited()
{
  return rendering_->commited;
}

}  // namespace zen::remote::client
