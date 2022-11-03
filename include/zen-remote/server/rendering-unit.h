#pragma once

#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IRenderingUnit {
  virtual ~IRenderingUnit() = default;
  virtual void GlEnableVertexAttribArray(uint32_t index) = 0;
  virtual void GlDisableVertexAttribArray(uint32_t index) = 0;
  virtual void GlVertexAttribPointer(uint32_t index, uint64_t buffer_id,
      int32_t size, uint64_t type, bool normalized, int32_t stride,
      uint64_t offset) = 0;
};

std::unique_ptr<IRenderingUnit> CreateRenderingUnit(
    std::shared_ptr<ISession> session, uint64_t virtual_object_id);

}  // namespace zen::remote::server
