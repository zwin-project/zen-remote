#pragma once

#include "core/common.h"
#include "zen-remote/server/rendering-unit.h"

namespace zen::remote::server {

class Remote;

class RenderingUnit final : public IRenderingUnit {
 public:
  DISABLE_MOVE_AND_COPY(RenderingUnit);
  RenderingUnit() = delete;
  RenderingUnit(std::shared_ptr<Remote> remote);
  ~RenderingUnit();

  void Init(uint64_t virtual_object_id);
  void GlEnableVertexAttribArray(uint32_t index) override;
  void GlDisableVertexAttribArray(uint32_t index) override;
  void GlVertexAttribPointer(uint32_t index, uint64_t buffer_id, int32_t size,
      uint64_t type, bool normalized, int32_t stride, uint64_t offset) override;
  uint64_t id() override;

 private:
  std::shared_ptr<Remote> remote_;
  uint64_t id_;
};

}  // namespace zen::remote::server
