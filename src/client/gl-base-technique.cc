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
  auto command =
      CreateCommand([render_mode = pending_.render_mode, this](bool cancel) {
        if (cancel) {
          return;
        }

        rendering_.render_mode = render_mode;
      });

  update_rendering_queue_->Push(std::move(command));

  pending_.damaged = false;
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  pending_.render_mode.render_method = RenderMethod::kArrays;
  pending_.render_mode.arrays.mode = mode;
  pending_.render_mode.arrays.count = count;
  pending_.render_mode.arrays.first = first;
  pending_.damaged = true;
}

void
GlBaseTechnique::Render()
{
  auto& mode = rendering_.render_mode;
  switch (mode.render_method) {
    case RenderMethod::kArrays:
      glDrawArrays(mode.arrays.mode, mode.arrays.first, mode.arrays.count);
      break;
  }
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

}  // namespace zen::remote::client
