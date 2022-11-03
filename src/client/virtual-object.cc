#include "client/virtual-object.h"

#include "client/atomic-command-queue.h"
#include "client/rendering-unit.h"
#include "zen-remote/client/camera.h"

namespace zen::remote::client {

VirtualObject::VirtualObject(
    uint64_t id, AtomicCommandQueue *update_rendering_queue)
    : id_(id), update_rendering_queue_(update_rendering_queue)
{
}

void
VirtualObject::Commit()
{
  ForEachWeakPtr<RenderingUnit>(pending_.rendering_units_,
      [](std::shared_ptr<RenderingUnit> unit) { unit->Commit(); });

  auto command = CreateCommand(
      [rendering_units = pending_.rendering_units_, this](bool cancel) {
        if (cancel) return;

        if (!rendering_.commited) rendering_.commited = true;
        rendering_.rendering_units_ = rendering_units;
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
  ForEachWeakPtr<RenderingUnit>(rendering_.rendering_units_,
      [camera](std::shared_ptr<RenderingUnit> unit) { unit->Render(camera); });
}

uint64_t
VirtualObject::id()
{
  return id_;
}

}  // namespace zen::remote::client
