#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;
class GlShader;

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

  /** Used in the update thread */
  void GlAttachShader(std::weak_ptr<GlShader> gl_shader);

  /** Used in the update thread */
  void GlLinkProgram();

  uint64_t id() override;

  /** Used in the rendering thread */
  inline GLuint program_id();

 private:
  const uint64_t id_;
  AtomicCommandQueue* update_rendering_queue_;

  struct {
    std::vector<std::weak_ptr<GlShader>> gl_shaders;  // Empty after linking
    bool should_link = false;
  } pending_;

  std::shared_ptr<RenderingState> rendering_;
};

inline GLuint
GlProgram::program_id()
{
  return rendering_->program_id;
}

}  // namespace zen::remote::client
