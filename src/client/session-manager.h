#pragma once

#include "core/common.h"

namespace zen::remote::client {

class Session;
class ResourcePool;

class SessionManager {
 public:
  DISABLE_MOVE_AND_COPY(SessionManager);
  SessionManager() = default;
  ~SessionManager();

  /** Used by any thread */
  void EnableSession();

  /** Used by any thread */
  void DisableSession();

  /** Used in the update thread */
  inline std::unique_ptr<Session>& current();

  /**
   * Destroy current session and set new session.
   * Used in the update thread
   *
   * @returns id of the new session. 0 when failed
   */
  uint64_t ResetCurrent();

  /**
   * Destroy current session and start discovery UDP broadcast
   * Used in the update thread
   */
  void ClearCurrent();

  /**
   * Used in the rendering thread
   *
   * @returns null if no current session exists.
   */
  std::shared_ptr<ResourcePool> GetCurrentResourcePool();

 private:
  void StartDiscoverBroadcast();
  void StopDiscoverBroadcast();

  // Always lock in this order:
  // current_mutex_ -> thread_mutex_ -> broadcast_.mutex

  std::unique_ptr<Session> current_;  // nullable
  bool current_enabled_ = false;
  bool current_is_valid_ = false;
  // lock `current_` (pointer only), `current_enabled_` and `current_id_valid_`
  std::mutex current_mutex_;

  struct {
    std::thread thread;
    // lock `thread` (joinable or not only)
    std::mutex thread_mutex;

    bool running;
    std::mutex mutex;  // lock `running`
    std::condition_variable cond;
  } broadcast_;
};

std::unique_ptr<Session>&
SessionManager::current()
{
  std::lock_guard<std::mutex> lock(current_mutex_);

  if (!current_is_valid_) current_.reset();

  return current_;
}

}  // namespace zen::remote::client
