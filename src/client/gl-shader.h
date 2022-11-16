#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

class GlShader final : public IResource {
  struct RenderingState {
    GLuint shader_id = 0;
  };

 public:
  DISABLE_MOVE_AND_COPY(GlShader);
  GlShader() = delete;
  GlShader(uint64_t id, std::string source, uint32_t type,
      AtomicCommandQueue *update_rendering_queue);
  ~GlShader();

  /** Used in the update thread */
  void Commit();

  uint64_t id() override;

  /** Used in the rendering thread */
  inline GLuint shader_id();

 private:
  const uint64_t id_;

  // become unspecified value after the inital commit
  const std::string source_;

  const uint32_t type_;
  AtomicCommandQueue *update_rendering_queue_;

  bool committed_ = false;
  std::shared_ptr<RenderingState> rendering_;
};

inline GLuint
GlShader::shader_id()
{
  return rendering_->shader_id;
}

}  // namespace zen::remote::client
