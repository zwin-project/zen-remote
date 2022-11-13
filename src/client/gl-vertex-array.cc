#include "client/gl-vertex-array.h"

#include "client/atomic-command-queue.h"

namespace zen::remote::client {

GlVertexArray::GlVertexArray(
    uint64_t id, AtomicCommandQueue *update_rendering_queue)
    : id_(id), update_rendering_queue_(update_rendering_queue)
{
}

void
GlVertexArray::Commit()
{
  // TODO:
  (void)update_rendering_queue_;
}

uint64_t
GlVertexArray::id()
{
  return id_;
}

}  // namespace zen::remote::client
