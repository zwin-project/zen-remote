#pragma once

#include "core/common.h"

namespace zen::remote::client {

class DiscoveryBroadcasts {
 public:
  DISABLE_MOVE_AND_COPY(DiscoveryBroadcasts);
  DiscoveryBroadcasts() = default;
  ~DiscoveryBroadcasts();

  void StartIfNotRunning();

  void StopAndWait();

 private:
  // If locking both mutexes, lock thread_mutex_ first.

  bool running_ = false;
  std::mutex mutex_;
  std::condition_variable cond_;

  std::thread thread_;
  std::mutex thread_mutex_;
};

}  // namespace zen::remote::client
