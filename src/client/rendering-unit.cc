#include "client/rendering-unit.h"

namespace zen::remote::client {

RenderingUnit::RenderingUnit(uint64_t id) : id_(id) {}

uint64_t
RenderingUnit::id()
{
  return id_;
}

}  // namespace zen::remote::client
