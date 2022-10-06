#pragma once

#include <stdint.h>

#include <functional>

namespace zen::remote {

struct FdSource {
  enum Mask {
    kReadable = 1 << 0,
    kWritable = 1 << 1,
    kHangup = 1 << 2,
    kError = 1 << 3,
  };

  int fd;
  uint32_t mask;
  std::function<void(int fd, uint32_t mask)> callback;
  void *data = nullptr;  // used by ILoop implementation
};

struct ILoop {
  virtual ~ILoop() = default;
  virtual void AddFd(FdSource *source) = 0;
  virtual void RemoveFd(FdSource *source) = 0;
  virtual void Terminate() = 0;
};

}  // namespace zen::remote
