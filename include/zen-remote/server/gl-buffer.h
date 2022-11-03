#pragma once

#include <zen-remote/server/buffer.h>
#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IGlBuffer {
  virtual ~IGlBuffer() = default;

  virtual void GlBufferData(std::unique_ptr<IBuffer> buffer, uint64_t target,
      size_t size, uint64_t usage) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlBuffer> CreateGlBuffer(std::shared_ptr<ISession> session);

}  // namespace zen::remote::server
