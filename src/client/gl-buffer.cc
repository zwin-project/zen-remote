#include "client/gl-buffer.h"

#include "client/atomic-command-queue.h"
#include "core/logger.h"

namespace zen::remote::client {

GlBuffer::GlBuffer(uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id), update_rendering_queue_(update_rendering_queue)
{
}

GlBuffer::~GlBuffer()
{
  if (rendering_.buffer_id != 0) {
    glDeleteBuffers(1, &rendering_.buffer_id);
  }

  free(pending_.data);
}

void
GlBuffer::Commit()
{
  if (pending_.data_damaged == false) return;

  void* data = malloc(pending_.size);
  memcpy(data, pending_.data, pending_.size);

  auto command =
      CreateCommand([data, target = pending_.target, size = pending_.size,
                        usage = pending_.usage, this]() {
        if (rendering_.buffer_id == 0) {
          glGenBuffers(1, &rendering_.buffer_id);
        }

        glBindBuffer(target, rendering_.buffer_id);
        glBufferData(target, size, data, usage);

        rendering_.target = target;

        free(data);
      });

  update_rendering_queue_->Push(std::move(command));

  pending_.data_damaged = false;
}

void
GlBuffer::GlBufferData(
    const void* data, uint64_t target, size_t size, uint64_t usage)
{
  if (size > pending_.alloc) {
    pending_.data = realloc(pending_.data, size);
    pending_.alloc = size;
  }

  memcpy(pending_.data, data, size);
  pending_.size = size;

  pending_.target = target;
  pending_.usage = usage;
  pending_.data_damaged = true;
}

uint64_t
GlBuffer::id()
{
  return id_;
}

}  // namespace zen::remote::client
