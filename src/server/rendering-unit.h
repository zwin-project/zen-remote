#pragma once

#include "core/common.h"
#include "zen-remote/server/rendering-unit.h"

namespace zen::remote::server {

class Channel;

class RenderingUnit final : public IRenderingUnit {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(std::shared_ptr<Channel> channel);
  ~RenderingUnit();

  void Init(uint64_t virtual_object_id);

  void ChangeVisibility(bool visible) override;

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Channel> channel_;
};

}  // namespace zen::remote::server
