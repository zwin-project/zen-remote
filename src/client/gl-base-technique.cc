#include "client/gl-base-technique.h"

#include "client/atomic-command-queue.h"
#include "core/logger.h"

namespace zen::remote::client {

GlBaseTechnique::GlBaseTechnique(
    uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id), update_rendering_queue_(update_rendering_queue)
{
}

GlBaseTechnique::~GlBaseTechnique() {}

void
GlBaseTechnique::Commit()
{
  auto command = CreateCommand([]() {});

  update_rendering_queue_->Push(std::move(command));
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  LOG_DEBUG("remote client: DrawArrays(%u, %d, %u)", mode, first, count);
}

void
GlBaseTechnique::Render()
{
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

}  // namespace zen::remote::client
