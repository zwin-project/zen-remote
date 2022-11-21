#pragma once

#include <zen-remote/server/buffer.h>
#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IGlTexture {
  virtual ~IGlTexture() = default;

  virtual void GlTexImage2D(uint32_t target, int32_t level,
      int32_t internal_format, uint32_t width, uint32_t height, int32_t border,
      uint32_t format, uint32_t type, std::unique_ptr<IBuffer> buffer) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlTexture> CreateGlTexture(std::shared_ptr<ISession> session);

}  // namespace zen::remote::server