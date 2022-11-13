#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;
struct Camera;

class GlBaseTechnique final : public IResource {
  enum class DrawMethod {
    kNone,
    kArrays,
  };

  union DrawArgs {
    struct {
      uint32_t mode;
      int32_t count;
      int32_t first;
    } arrays;
  };

  struct RenderingState {
    DrawArgs draw_args;
    DrawMethod draw_method = DrawMethod::kNone;
  };

 public:
  DISABLE_MOVE_AND_COPY(GlBaseTechnique);
  GlBaseTechnique() = delete;
  GlBaseTechnique(uint64_t id, AtomicCommandQueue *update_rendering_queue);
  ~GlBaseTechnique();

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void GlDrawArrays(uint32_t mode, int32_t first, uint32_t count);

  /** Used in the rendering thread */
  void Render(Camera *camera);

  uint64_t id() override;

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    bool damaged = false;

    DrawArgs draw_args;
    DrawMethod draw_method = DrawMethod::kNone;
  } pending_;

  std::shared_ptr<RenderingState> rendering_;
};

}  // namespace zen::remote::client
