#include "client/rendering-unit.h"

#include "client/atomic-command-queue.h"
#include "client/gl-buffer.h"

namespace zen::remote::client {

RenderingUnit::RenderingUnit(
    uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id), update_rendering_queue_(update_rendering_queue)
{
}

void
RenderingUnit::Commit()
{
  for (auto& vertex_attrib : pending_.vertex_attribs) {
    if (auto gl_buffer = vertex_attrib.second.gl_buffer.lock()) {
      gl_buffer->Commit();
    }
  }

  // TODO: commit pending state
  (void)rendering_;
  (void)update_rendering_queue_;
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
