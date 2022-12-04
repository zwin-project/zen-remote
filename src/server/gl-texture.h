#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-texture.h"

namespace zen::remote::server {

class Session;

class GlTexture final : public IGlTexture {
 public:
  DISABLE_MOVE_AND_COPY(GlTexture);
  GlTexture() = delete;
  GlTexture(std::shared_ptr<Session> session);
  ~GlTexture();

  void Init();

  void GlTexImage2D(uint32_t target, int32_t level, int32_t internal_format,
      uint32_t width, uint32_t height, int32_t border, uint32_t format,
      uint32_t type, std::unique_ptr<IBuffer> buffer) override;

  void GlTexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
      int32_t yoffset, uint32_t width, uint32_t height, uint32_t format,
      uint32_t type, std::unique_ptr<IBuffer> buffer) override;

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Session> session_;
};

}  // namespace zen::remote::server
