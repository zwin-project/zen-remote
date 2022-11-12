#pragma once

#include "core/common.h"
#include "zen-remote/server/rendering-unit.h"

namespace zen::remote::server {

class Session;

class RenderingUnit final : public IRenderingUnit {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(std::shared_ptr<Session> session);
  ~RenderingUnit();

  void Init(uint64_t virtual_object_id);
  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Session> session_;
};

}  // namespace zen::remote::server
