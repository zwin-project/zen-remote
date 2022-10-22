#pragma once

#include "core/common.h"
#include "zen-remote/server/buffer.h"

namespace zen::remote::server {

class Remote;

/**
 * No matter which thread destroys this buffer, the `on_release` callback is
 * properly called
 */
class Buffer final : public IBuffer {
 public:
  DISABLE_MOVE_AND_COPY(Buffer);
  Buffer() = delete;
  Buffer(void *data, std::function<void()> on_release,
      std::shared_ptr<Remote> remote);
  ~Buffer();

  bool Init();

  void *data() override;

 private:
  void *data_;
  std::function<void()> on_release_;
  std::shared_ptr<Remote> remote_;

  int pipe_[2];
};

}  // namespace zen::remote::server
