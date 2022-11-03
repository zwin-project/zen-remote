#include "client/rendering-unit.h"

#include "client/atomic-command-queue.h"
#include "client/gl-buffer.h"
#include "client/tmp-rendering-helper.h"
#include "zen-remote/client/camera.h"

namespace zen::remote::client {

RenderingUnit::RenderingUnit(
    uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id), update_rendering_queue_(update_rendering_queue)
{
}

RenderingUnit::~RenderingUnit()
{
  if (rendering_.vao != 0) {
    glDeleteVertexArrays(1, &rendering_.vao);
  }
}

void
RenderingUnit::Commit()
{
  for (auto& vertex_attrib : pending_.vertex_attribs) {
    if (auto gl_buffer = vertex_attrib.second.gl_buffer.lock()) {
      gl_buffer->Commit();
    }
  }

  auto command = CreateCommand([attribs = pending_.vertex_attribs, this]() {
    if (rendering_.vao == 0) {
      glGenVertexArrays(1, &rendering_.vao);
      rendering_.program_id = TmpRenderingHelper::CompilePrograms(
          default_vertex_shader, default_color_fragment_shader);
    }

    glBindVertexArray(rendering_.vao);
    for (auto& [index, attrib] : attribs) {
      if (attrib.filled == false || attrib.enabled == false) {
        glDisableVertexAttribArray(index);
        continue;
      }

      if (auto buffer = attrib.gl_buffer.lock()) {
        glEnableVertexAttribArray(index);

        glBindBuffer(buffer->target(), buffer->buffer_id());
        glVertexAttribPointer(index, attrib.size, attrib.type,
            attrib.normalized, attrib.stride, (void*)attrib.offset);

        glBindBuffer(buffer->target(), 0);
      } else {
        glDisableVertexAttribArray(index);
      }
    }
    glBindVertexArray(0);
  });

  update_rendering_queue_->Push(std::move(command));
}

void
RenderingUnit::SetGlBaseTechnique(
    std::weak_ptr<GlBaseTechnique> gl_base_technique)
{
  gl_base_technique_ = gl_base_technique;
}

void
RenderingUnit::GlEnableVertexAttribArray(uint32_t index)
{
  auto result = pending_.vertex_attribs.find(index);

  if (result != pending_.vertex_attribs.end()) {
    (*result).second.enabled = true;
  } else {
    pending_.vertex_attribs.emplace(std::piecewise_construct,
        std::forward_as_tuple(index), std::forward_as_tuple(index));
  }
}

void
RenderingUnit::GlDisableVertexAttribArray(uint32_t index)
{
  auto result = pending_.vertex_attribs.find(index);
  if (result == pending_.vertex_attribs.end()) return;

  (*result).second.enabled = false;
}

void
RenderingUnit::GlVertexAttribPointer(uint32_t index,
    std::weak_ptr<GlBuffer> gl_buffer, int32_t size, uint64_t type,
    bool normalized, int32_t stride, uint64_t offset)
{
  auto result = pending_.vertex_attribs.find(index);
  if (result == pending_.vertex_attribs.end()) return;

  (*result).second.index = index;
  (*result).second.gl_buffer = gl_buffer;
  (*result).second.size = size;
  (*result).second.type = type;
  (*result).second.normalized = normalized;
  (*result).second.stride = stride;
  (*result).second.offset = offset;
  (*result).second.filled = true;
}

void
RenderingUnit::Render(Camera* camera)
{
  glBindVertexArray(rendering_.vao);
  glUseProgram(rendering_.program_id);
  GLint mvp_location = glGetUniformLocation(rendering_.program_id, "mvp");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (float*)&camera->vp);

  auto gl_base_technique = gl_base_technique_.lock();
  if (gl_base_technique) {
    gl_base_technique->Render();
  }

  glBindVertexArray(0);
  glUseProgram(0);
}

uint64_t
RenderingUnit::id()
{
  return id_;
}

RenderingUnit::VertexAttrib::VertexAttrib(uint32_t index)
    : index(index), enabled(true), filled(false)
{
}

}  // namespace zen::remote::client
