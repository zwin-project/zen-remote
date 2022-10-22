#pragma once

#include <zen-remote/server/buffer.h>
#include <zen-remote/server/remote.h>

#include <memory>

namespace zen::remote::server {

struct IGlBuffer {
  virtual ~IGlBuffer() = default;

  virtual void GlBufferData(
      std::unique_ptr<IBuffer> buffer, size_t size, uint64_t usage) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlBuffer> CreateGlBuffer(std::shared_ptr<IRemote> remote);

}  // namespace zen::remote::server
