#pragma once

#include "client/resource.h"
#include "core/common.h"
#include "zen-remote/client/rendering-unit.h"

namespace zen::remote::client {

class GlBuffer;
class AtomicCommandQueue;

class RenderingUnit final : public IRenderingUnit, public IResource {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(uint64_t id, AtomicCommandQueue *update_rendering_queue);

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void GlEnableVertexAttribArray(uint32_t index);

  /** Used in the update thread */
  void GlDisableVertexAttribArray(uint32_t index);

  /** Used in the update thread */
  void GlVertexAttribPointer(uint32_t index, std::weak_ptr<GlBuffer> gl_buffer,
      int32_t size, uint64_t type, bool normalized, int32_t stride,
      uint64_t offset);

  uint64_t id() override;

 private:
  struct VertexAttrib {
    VertexAttrib(uint32_t index);

    uint64_t type;
    uint64_t offset;
    uint32_t index;
    int32_t size;
    int32_t stride;
    std::weak_ptr<GlBuffer> gl_buffer;
    bool normalized;
    bool enabled;
    bool filled;
  };

  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    std::unordered_map<uint32_t, VertexAttrib> vertex_attribs;
  } pending_;

  struct {
    // TODO:
  } rendering_;
};

}  // namespace zen::remote::client
