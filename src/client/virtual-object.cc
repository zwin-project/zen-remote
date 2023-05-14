#include "client/virtual-object.h"

#include "client/atomic-command-queue.h"
#include "client/rendering-unit.h"
#include "zen-remote/client/camera.h"

namespace zen::remote::client {

VirtualObject::VirtualObject(
    uint64_t id, AtomicCommandQueue *update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

VirtualObject::~VirtualObject()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {});

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
VirtualObject::Commit()
{
  ForEachWeakPtr<RenderingUnit>(pending_.rendering_units_,
      [](std::shared_ptr<RenderingUnit> unit) { unit->Commit(); });

  auto command = CreateCommand([rendering_units = pending_.rendering_units_,
                                   rendering = rendering_](bool cancel) {
    if (cancel) return;

    if (!rendering->commited) rendering->commited = true;
    rendering->rendering_units_ = rendering_units;
  });

  update_rendering_queue_->Push(std::move(command));

  update_rendering_queue_->Commit();
}

void
VirtualObject::Move(glm::vec3 position, glm::quat quaternion)
{
  auto command = CreateCommand(
      [rendering = rendering_, position, quaternion](bool cancel) {
        if (cancel) return;
        rendering->position = position;
        rendering->quaternion = quaternion;
      });

  update_rendering_queue_->Push(std::move(command));

  update_rendering_queue_->Commit();
}

void
VirtualObject::ChangeVisibility(bool visible)
{
  auto command = CreateCommand([rendering = rendering_, visible](bool cancel) {
    if (cancel) return;

    rendering->visible = visible;
  });

  update_rendering_queue_->Push(std::move(command));

  update_rendering_queue_->Commit();
}

void
VirtualObject::AddRenderingUnit(std::weak_ptr<RenderingUnit> rendering_unit)
{
  pending_.rendering_units_.push_back(rendering_unit);
}

void
VirtualObject::Render(Camera *camera)
{
  if (!rendering_->visible) {
    return;
  }

  glm::mat4 rotate = glm::toMat4(rendering_->quaternion);
  glm::mat4 translate = glm::translate(glm::mat4(1.0f), rendering_->position);
  glm::mat4 model = translate * rotate;

  ForEachWeakPtr<RenderingUnit>(rendering_->rendering_units_,
      [camera, &model](std::shared_ptr<RenderingUnit> unit) {
        unit->Render(camera, model);
      });
}

uint64_t
VirtualObject::id()
{
  return id_;
}

}  // namespace zen::remote::client
