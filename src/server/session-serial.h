#pragma once

#include "core/common.h"

namespace zen::remote::server {

class SessionSerial {
 public:
  DISABLE_MOVE_AND_COPY(SessionSerial);
  SessionSerial() = default;
  ~SessionSerial() = default;

  uint64_t NextResourceSerial();

  uint64_t NextChannelSerial();

 private:
  struct {
    uint64_t serial = 1;
    std::mutex mtx;
  } resource_;

  struct {
    uint64_t serial = 1;
    std::mutex mtx;
  } channel_;
};

}  // namespace zen::remote::server
