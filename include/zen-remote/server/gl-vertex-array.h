#pragma once

#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IGlVertexArray {
  virtual ~IGlVertexArray() = default;

  virtual void GlEnableVertexAttribArray(uint32_t index) = 0;

  virtual void GlDisableVertexAttribArray(uint32_t index) = 0;

  virtual void GlVertexAttribPointer(uint32_t index, int32_t size,
      uint32_t type, bool normalized, int32_t stride, uint64_t offset,
      uint64_t gl_buffer_id) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlVertexArray> CreateGlVertexArray(
    std::shared_ptr<ISession> session);

}  // namespace zen::remote::server
