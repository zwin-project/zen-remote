#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-shader.h"

namespace zen::remote::server {

class Session;

class GlShader final : public IGlShader {
 public:
  DISABLE_MOVE_AND_COPY(GlShader);
  GlShader() = delete;
  GlShader(std::shared_ptr<Session> session);
  ~GlShader();

  void Init(std::string source, uint32_t type);

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Session> session_;
};

}  // namespace zen::remote::server
