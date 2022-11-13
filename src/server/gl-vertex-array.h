#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-vertex-array.h"

namespace zen::remote::server {

class Session;

class GlVertexArray final : public IGlVertexArray {
 public:
  DISABLE_MOVE_AND_COPY(GlVertexArray);
  GlVertexArray() = delete;
  GlVertexArray(std::shared_ptr<Session> session);
  ~GlVertexArray();

  void Init();

 private:
  uint64_t id_;
  std::weak_ptr<Session> session_;
};

}  // namespace zen::remote::server
