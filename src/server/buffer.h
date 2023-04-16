#pragma once

#include "core/common.h"
#include "zen-remote/loop.h"
#include "zen-remote/server/buffer.h"

namespace zen::remote::server {

/**
 * No matter which thread destroys this buffer, the `on_release` callback is
 * properly called
 */
class Buffer final : public IBuffer {
 public:
  DISABLE_MOVE_AND_COPY(Buffer);
  Buffer() = delete;
  Buffer(std::function<void *()> on_begin_access,
      std::function<bool()> on_end_access, std::function<void()> on_release,
      std::unique_ptr<ILoop> loop);
  ~Buffer();

  bool Init();

  void *begin_access() override;

  bool end_access() override;

 private:
  std::function<void *()> on_begin_access_;
  std::function<bool()> on_end_access_;
  std::function<void()> on_release_;
  std::shared_ptr<ILoop> loop_;

  int pipe_[2];
};

}  // namespace zen::remote::server
