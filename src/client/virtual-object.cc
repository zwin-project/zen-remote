#include "client/virtual-object.h"

#include "client/atomic-command-queue.h"
#include "client/rendering-unit.h"

namespace zen::remote::client {

VirtualObject::VirtualObject(
    uint64_t id, AtomicCommandQueue *update_rendering_queue)
    : id_(id), update_rendering_queue_(update_rendering_queue)
{
}

uint64_t
VirtualObject::id()
{
  return id_;
}

void
VirtualObject::ForEachRenderingUnit(std::function<void(IRenderingUnit *)> func)
{
  ForEachWeakPtr<std::list, RenderingUnit>(rendering_.rendering_units_,
      [func](std::shared_ptr<RenderingUnit> unit) { func(unit.get()); });
}

void
VirtualObject::Commit()
{
  ForEachWeakPtr<std::list, RenderingUnit>(pending_.rendering_units_,
      [](std::shared_ptr<RenderingUnit> unit) { unit->Commit(); });

  auto command = std::make_unique<Command>(
      [rendering_units = pending_.rendering_units_, this]() {
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

}  // namespace zen::remote::client
