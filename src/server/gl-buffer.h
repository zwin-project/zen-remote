#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-buffer.h"

namespace zen::remote::server {

class Remote;

class GlBuffer final : public IGlBuffer {
 public:
  DISABLE_MOVE_AND_COPY(GlBuffer);
  GlBuffer() = delete;
  GlBuffer(std::shared_ptr<Remote> remote);
  ~GlBuffer();

  void Init();

  uint64_t id() override;

 private:
  std::shared_ptr<Remote> remote_;
  uint64_t id_;
};

}  // namespace zen::remote::server
