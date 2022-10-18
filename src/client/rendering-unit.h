#pragma once

#include "client/resource.h"
#include "core/common.h"
#include "zen-remote/client/rendering-unit.h"

namespace zen::remote::client {

class RenderingUnit final : public IRenderingUnit, public IResource {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(uint64_t id) : id_(id){};

  uint64_t id() override;

 private:
  const uint64_t id_;
  bool commited_ = false;  // false until the first commit
  bool update_ = false;    // true when pending changes need to be applied
};

}  // namespace zen::remote::client
