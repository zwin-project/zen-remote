#include "client/gl-program.h"

#include "client/atomic-command-queue.h"

namespace zen::remote::client {

GlProgram::GlProgram(uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState)
{
}

GlProgram::~GlProgram()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {
    if (rendering->program_id != 0) {
      glDeleteProgram(rendering->program_id);
    }
  });

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
GlProgram::Commit()
{
  if (pending_.damaged == false) return;

  auto command = CreateCommand([rendering = rendering_](bool cancel) {
    if (cancel) {
      return;
    }

    if (rendering->program_id == 0) {
      rendering->program_id = glCreateProgram();
    }
  });

  update_rendering_queue_->Push(std::move(command));

  pending_.damaged = false;
}

uint64_t
GlProgram::id()
{
  return id_;
}

}  // namespace zen::remote::client
