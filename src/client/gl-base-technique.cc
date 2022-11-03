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
  if (pending_.damaged == false) return;

  auto command = CreateCommand([render_mode = pending_.render_mode, this]() {
    rendering_.render_mode = render_mode;
  });

  update_rendering_queue_->Push(std::move(command));

  pending_.damaged = false;
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  pending_.render_mode.render_method = RenderMethod::kArrays;
  pending_.render_mode.mode = mode;
  pending_.render_mode.count = count;
  pending_.render_mode.first = first;
  pending_.damaged = true;
}

void
GlBaseTechnique::Render()
{
  switch (rendering_.render_mode.render_method) {
    case RenderMethod::kArrays:
      glDrawArrays(rendering_.render_mode.mode, rendering_.render_mode.first,
          rendering_.render_mode.count);
      break;
  }
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

}  // namespace zen::remote::client
