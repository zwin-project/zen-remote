#include "client/gl-buffer.h"

#include "core/logger.h"

namespace zen::remote::client {

GlBuffer::GlBuffer(uint64_t id) : id_(id) {}

void
GlBuffer::Commit()
{
  // TODO: commit pending state
}

void
GlBuffer::GlBufferData(const void *data, size_t size, uint64_t usage)
{
  // TODO: to store data, we have to copy it.
  (void)usage;
  auto f = static_cast<const float *>(data);
  LOG_INFO("GlBufferData");
  for (uint i = 0; i < size / sizeof(float); i += 3) {
    LOG_INFO("  (%f, %f, %f)", f[i], f[i + 1], f[i + 2]);
  }
}

uint64_t
GlBuffer::id()
{
  return id_;
}

}  // namespace zen::remote::client
