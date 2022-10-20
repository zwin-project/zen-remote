#include "client/virtual-object.h"

#include "client/rendering-unit.h"

namespace zen::remote::client {

VirtualObject::VirtualObject(uint64_t id) : id_(id) {}

uint64_t
VirtualObject::id()
{
  return id_;
}

void
VirtualObject::ForEachRenderingUnit(
    std::function<void(IRenderingUnit*)> /*func*/)
{
  // TODO:
}

void
VirtualObject::AddRenderingUnit(std::weak_ptr<RenderingUnit> /*rendering_unit*/)
{
  // TODO:
}

}  // namespace zen::remote::client
