#pragma once

#include "client/gl-base-technique.h"
#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class GlBuffer;
class AtomicCommandQueue;
struct Camera;

class RenderingUnit final : public IResource {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(uint64_t id, AtomicCommandQueue *update_rendering_queue);
  ~RenderingUnit();

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void SetGlBaseTechnique(std::weak_ptr<GlBaseTechnique> gl_base_technique);

  /** Used in the update thread */
  void GlEnableVertexAttribArray(uint32_t index);

  /** Used in the update thread */
  void GlDisableVertexAttribArray(uint32_t index);

  /** Used in the update thread */
  void GlVertexAttribPointer(uint32_t index, std::weak_ptr<GlBuffer> gl_buffer,
      int32_t size, uint64_t type, bool normalized, int32_t stride,
      uint64_t offset);

  /** Used in the rendering thread */
  void Render(Camera *camera);

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

  std::weak_ptr<GlBaseTechnique> gl_base_technique_;

  struct {
    std::unordered_map<uint32_t, VertexAttrib> vertex_attribs;
  } pending_;

  struct {
    GLuint vao = 0;
    GLuint program_id;  // FIXME: use requested one
  } rendering_;
};

}  // namespace zen::remote::client
