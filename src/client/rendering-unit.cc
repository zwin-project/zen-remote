#include "client/rendering-unit.h"

#include "client/atomic-command-queue.h"
#include "client/gl-base-technique.h"
#include "client/gl-buffer.h"
#include "zen-remote/client/camera.h"

namespace zen::remote::client {

RenderingUnit::RenderingUnit(
    uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

RenderingUnit::~RenderingUnit()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {});

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
RenderingUnit::Commit()
{
  if (auto gl_base_technique = pending_.gl_base_technique.lock()) {
    gl_base_technique->Commit();
  }

  auto command = CreateCommand(
      [technique = pending_.gl_base_technique, visible = pending_.visible,
          rendering = rendering_](bool cancel) {
        if (cancel) {
          return;
        }

        rendering->gl_base_technique = technique;
        rendering->visible = visible;
      });

  update_rendering_queue_->Push(std::move(command));
}

void
RenderingUnit::SetGlBaseTechnique(
    std::weak_ptr<GlBaseTechnique> gl_base_technique)
{
  pending_.gl_base_technique = gl_base_technique;
}

void
RenderingUnit::ChangeVisibility(bool visible)
{
  pending_.visible = visible;
}

void
RenderingUnit::Render(Camera* camera, const glm::mat4& model)
{
  if (!rendering_->visible) {
    return;
  }

  if (auto gl_base_technique = rendering_->gl_base_technique.lock()) {
    gl_base_technique->Render(camera, model);
  }
}

uint64_t
RenderingUnit::id()
{
  return id_;
}

}  // namespace zen::remote::client
