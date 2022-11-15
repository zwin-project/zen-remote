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

  void Commit();

  uint64_t id() override;

 private:
  const uint64_t id_;

  // become unspecified value after the inital commit
  const std::string source_;

  const uint32_t type_;
  AtomicCommandQueue *update_rendering_queue_;

  bool committed_ = false;
  std::shared_ptr<RenderingState> rendering_;
};

}  // namespace zen::remote::client
