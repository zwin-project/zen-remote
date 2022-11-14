#include "client/gl-vertex-array.h"

#include "client/atomic-command-queue.h"
#include "core/logger.h"

namespace zen::remote::client {

GlVertexArray::GlVertexArray(
    uint64_t id, AtomicCommandQueue *update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

GlVertexArray::~GlVertexArray()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {
    if (rendering->vertex_array_id != 0) {
      glDeleteVertexArrays(1, &rendering->vertex_array_id);
    }
  });

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
GlVertexArray::Commit()
{
  if (pending_.damaged == false) return;

  for (auto &[_, attribute] : pending_.attributes) {
    if (auto gl_buffer = attribute.gl_buffer.lock()) {
      gl_buffer->Commit();
    }
  }

  auto command = CreateCommand(
      [attributes = pending_.attributes, rendering = rendering_](bool cancel) {
        if (cancel) {
          return;
        }

        if (rendering->vertex_array_id == 0) {
          glGenVertexArrays(1, &rendering->vertex_array_id);
        }

        glBindVertexArray(rendering->vertex_array_id);

        for (auto &[index, attribute] : attributes) {
          if (attribute.damaged == false) continue;

          if (attribute.enabled) {
            glEnableVertexAttribArray(index);
          } else {
            glDisableVertexAttribArray(index);
          }

          if (!attribute.filled) continue;

          auto gl_buffer = attribute.gl_buffer.lock();
          if (!gl_buffer) continue;

          glBindBuffer(gl_buffer->target(), gl_buffer->buffer_id());

          glVertexAttribPointer(index, attribute.size, attribute.type,
              attribute.normalized, attribute.stride, (void *)attribute.offset);

          glBindBuffer(gl_buffer->target(), 0);
        }

        glBindVertexArray(0);
      });

  update_rendering_queue_->Push(std::move(command));

  pending_.damaged = false;
  for (auto &[index, attribute] : pending_.attributes) {
    attribute.damaged = false;
  }
}

void
GlVertexArray::GlEnableVertexAttribArray(uint32_t index)
{
  VertexAttrib attribute;

  auto [result, _] =
      pending_.attributes.insert(std::make_pair(index, attribute));

  (*result).second.enabled = true;
  (*result).second.damaged = true;

  pending_.damaged = true;
}

void
GlVertexArray::GlDisableVertexAttribArray(uint32_t index)
{
  VertexAttrib attribute;

  auto [result, _] =
      pending_.attributes.insert(std::make_pair(index, attribute));

  (*result).second.enabled = false;
  (*result).second.damaged = true;

  pending_.damaged = true;
}

void
GlVertexArray::GlVertexAttribPointer(uint32_t index, int32_t size,
    uint32_t type, bool normalized, int32_t stride, uint64_t offset,
    std::weak_ptr<GlBuffer> gl_buffer)
{
  VertexAttrib attribute;

  auto [result, _] =
      pending_.attributes.insert(std::make_pair(index, attribute));

  (*result).second.size = size;
  (*result).second.type = type;
  (*result).second.normalized = normalized;
  (*result).second.stride = stride;
  (*result).second.offset = offset;
  (*result).second.gl_buffer = gl_buffer;
  (*result).second.filled = true;
  (*result).second.damaged = true;

  pending_.damaged = true;
}

uint64_t
GlVertexArray::id()
{
  return id_;
}

}  // namespace zen::remote::client
