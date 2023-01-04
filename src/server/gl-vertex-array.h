#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-vertex-array.h"

namespace zen::remote::server {

class Channel;

class GlVertexArray final : public IGlVertexArray {
 public:
  DISABLE_MOVE_AND_COPY(GlVertexArray);
  GlVertexArray() = delete;
  GlVertexArray(std::shared_ptr<Channel> channel);
  ~GlVertexArray();

  void Init();

  void GlEnableVertexAttribArray(uint32_t index) override;

  void GlDisableVertexAttribArray(uint32_t index) override;

  void GlVertexAttribPointer(uint32_t index, int32_t size, uint32_t type,
      bool normalized, int32_t stride, uint64_t offset,
      uint64_t gl_buffer_id) override;

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Channel> channel_;
};

}  // namespace zen::remote::server
