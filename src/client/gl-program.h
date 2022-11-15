#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

class GlProgram final : public IResource {
  struct RenderingState {
    GLuint program_id = 0;
  };

 public:
  DISABLE_MOVE_AND_COPY(GlProgram);
  GlProgram() = delete;
  GlProgram(uint64_t id, AtomicCommandQueue* update_rendering_queue);
  ~GlProgram();

  /** Used in the update thread */
  void Commit();

  uint64_t id() override;

 private:
  const uint64_t id_;
  AtomicCommandQueue* update_rendering_queue_;

  struct {
    // FIXME: specify which attribute is damaged / default false
    bool damaged = true;
  } pending_;

  std::shared_ptr<RenderingState> rendering_;
};

}  // namespace zen::remote::client
