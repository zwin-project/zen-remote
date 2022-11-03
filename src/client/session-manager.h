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

  /** Used in the rendering thread */
  bool Start();

  /** Used in the update thread */
  inline std::unique_ptr<Session>& current();

  /**
   * Destroy current session and set new session.
   * Used in the update thread
   */
  std::unique_ptr<Session>& ResetCurrent();

  /**
   * Used in the rendering thread
   *
   * @returns null if no current session exists.
   */
  std::shared_ptr<ResourcePool> GetCurrentResourcePool();

 private:
  void StartDiscoverBroadcast();
  void StopDiscoverBroadcast();

  struct {
    std::thread thread;
    bool running;
    std::mutex mutex;
    std::condition_variable cond;
  } broadcast_;

  std::unique_ptr<Session> current_;  // nullable
  std::mutex current_mutex_;
};

std::unique_ptr<Session>&
SessionManager::current()
{
  return current_;
}

}  // namespace zen::remote::client
