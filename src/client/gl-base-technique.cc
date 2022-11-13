#include "client/gl-base-technique.h"

#include "client/atomic-command-queue.h"
#include "zen-remote/client/camera.h"

namespace zen::remote::client {

GlBaseTechnique::GlBaseTechnique(
    uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

GlBaseTechnique::~GlBaseTechnique()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {});

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
GlBaseTechnique::Commit()
{
  if (pending_.damaged == false) return;
  auto command =
      CreateCommand([args = pending_.draw_args, method = pending_.draw_method,
                        rendering = rendering_](bool cancel) {
        if (cancel) {
          return;
        }

        rendering->draw_args = args;
        rendering->draw_method = method;
      });

  update_rendering_queue_->Push(std::move(command));

  pending_.damaged = false;
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  pending_.draw_method = DrawMethod::kArrays;
  pending_.draw_args.arrays.mode = mode;
  pending_.draw_args.arrays.count = count;
  pending_.draw_args.arrays.first = first;
  pending_.damaged = true;
}

void
GlBaseTechnique::Render(Camera* /*camera*/)
{
  switch (rendering_->draw_method) {
    case DrawMethod::kArrays: {
      auto args = rendering_->draw_args.arrays;
      glDrawArrays(args.mode, args.first, args.count);
      break;
    }
  }
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

}  // namespace zen::remote::client
