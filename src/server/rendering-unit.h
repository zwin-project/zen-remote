#pragma once

#include "core/common.h"
#include "server/remote.h"
#include "zen-remote/server/rendering-unit.h"

namespace zen::remote::server {

class RenderingUnit final : public IRenderingUnit {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(std::shared_ptr<Remote> remote)
      : remote_(std::move(remote)),
        id_(remote_->NewSerial(Remote::SerialType::kResource))
  {
  }
  ~RenderingUnit();

  void Init();

 private:
  std::shared_ptr<Remote> remote_;
  uint64_t id_;
};

}  // namespace zen::remote::server
