#pragma once

#include "client/resource.h"
#include "core/common.h"
#include "zen-remote/client/gl-buffer.h"

namespace zen::remote::client {

class GlBuffer final : public IGlBuffer, public IResource {
 public:
  DISABLE_MOVE_AND_COPY(GlBuffer);
  GlBuffer() = delete;
  GlBuffer(uint64_t id);

  /** Used in the update thread */
  void Commit();

  uint64_t id() override;

 private:
  const uint64_t id_;
};

}  // namespace zen::remote::client
