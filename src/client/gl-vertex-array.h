#pragma once

#include "client/gl-buffer.h"
#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

class GlVertexArray final : public IResource {
  struct VertexAttrib {
    std::weak_ptr<GlBuffer> gl_buffer;
    uint64_t offset;
    int32_t size;
    uint32_t type;
    int32_t stride;
    bool normalized;
    bool enabled = false;
    bool filled = false;
    bool damaged = false;
  };

  struct RenderingState {
    GLuint vertex_array_id = 0;
  };

 public:
  DISABLE_MOVE_AND_COPY(GlVertexArray);
  GlVertexArray() = delete;
  GlVertexArray(uint64_t id, AtomicCommandQueue *update_rendering_queue);
  ~GlVertexArray();

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void GlEnableVertexAttribArray(uint32_t index);

  /** Used in the update thread */
  void GlDisableVertexAttribArray(uint32_t index);

  /** Used in the update thread */
  void GlVertexAttribPointer(uint32_t index, int32_t size, uint32_t type,
      bool normalized, int32_t stride, uint64_t offset,
      std::weak_ptr<GlBuffer> gl_buffer);

  uint64_t id() override;

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    std::unordered_map<uint32_t, VertexAttrib> attributes;
    bool damaged = false;
  } pending_;

  std::shared_ptr<RenderingState> rendering_;
};

}  // namespace zen::remote::client
