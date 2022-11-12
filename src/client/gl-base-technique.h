#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

class GlBaseTechnique final : public IResource {
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
  void Render();

  uint64_t id() override;

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  enum class DrawMethod {
    kArrays,
  };

  union DrawArgs {
    struct {
      uint32_t mode;
      int32_t count;
      int32_t first;
    } arrays;
  };

  struct {
    bool damaged = false;

    DrawArgs draw_args;
    DrawMethod draw_method;
  } pending_;

  struct {
    DrawArgs draw_args;
    DrawMethod draw_method;
  } rendering_;
};

}  // namespace zen::remote::client
