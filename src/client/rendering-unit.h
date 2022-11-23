#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class GlBuffer;
class AtomicCommandQueue;
class GlBaseTechnique;
struct Camera;

class RenderingUnit final : public IResource {
  struct RenderingState {
    std::weak_ptr<GlBaseTechnique> gl_base_technique;
  };

 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(uint64_t id, AtomicCommandQueue *update_rendering_queue);
  ~RenderingUnit();

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void SetGlBaseTechnique(std::weak_ptr<GlBaseTechnique> gl_base_technique);

  /** Used in the rendering thread */
  void Render(Camera *camera, const glm::mat4 &model);

  uint64_t id() override;

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    std::weak_ptr<GlBaseTechnique> gl_base_technique;
  } pending_;

  std::shared_ptr<RenderingState> rendering_;  // nonnull
};

}  // namespace zen::remote::client
