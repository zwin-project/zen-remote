#include "client/gl-buffer.h"

namespace zen::remote::client {

GlBuffer::GlBuffer(uint64_t id) : id_(id) {}

void
GlBuffer::Commit()
{
  // TODO: commit pending state
}

uint64_t
GlBuffer::id()
{
  return id_;
}

}  // namespace zen::remote::client
