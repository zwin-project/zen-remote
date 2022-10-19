#pragma once

#include "client/resource.h"
#include "core/common.h"
#include "zen-remote/client/rendering-unit.h"

namespace zen::remote::client {

class RenderingUnit final : public IRenderingUnit, public IResource {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(uint64_t id);

  uint64_t id() override;

 private:
  const uint64_t id_;
};

}  // namespace zen::remote::client
