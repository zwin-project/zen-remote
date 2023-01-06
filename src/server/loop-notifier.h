#pragma once

#include "core/common.h"
#include "zen-remote/loop.h"

namespace zen::remote::server {

struct ILoopNotifierDelegate {
  virtual ~ILoopNotifierDelegate() = default;

  virtual void Notify(uint8_t signal) = 0;
};

class LoopNotifier {
 public:
  DISABLE_MOVE_AND_COPY(LoopNotifier);
  LoopNotifier() = delete;
  /**
   * @param delegate must not be null, and its life time must be longer than
   * this LoopNotifier
   */
  LoopNotifier(std::shared_ptr<ILoop> loop, ILoopNotifierDelegate *delegate);
  ~LoopNotifier();

  bool Init();

  void Notify(uint8_t signal);

 private:
  void Callback(int fd, uint32_t mask);

  int pipe_[2];
  std::shared_ptr<ILoop> loop_;
  ILoopNotifierDelegate *delegate_;
  std::unique_ptr<FdSource> event_source_;
};

}  // namespace zen::remote::server
