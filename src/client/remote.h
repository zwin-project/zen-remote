#pragma once

#include "client/atomic-command-queue.h"
#include "client/discovery-broadcast.h"
#include "core/common.h"
#include "zen-remote/client/remote.h"

namespace zen::remote::client {

class GrpcServer;
class Session;

class Remote : public IRemote {
 public:
  DISABLE_MOVE_AND_COPY(Remote);
  Remote() = delete;
  Remote(std::unique_ptr<ILoop> loop);
  ~Remote();

  void StartGrpcServer() override;

  void EnableSession() override;

  void DisableSession() override;

  /** Call only once before rendering for multiple cameras. */
  void UpdateScene() override;

  /** Used from any threads */
  std::shared_ptr<Session> ResetSession();

  /** Used from any threads */
  void ClearSession(uint64_t id);

  /** Used from any threads */
  bool IsNewSessionAcceptable();

  /**
   * Before calling this, bind a framebuffer, set a viewport, and clear the
   * framebuffer
   */
  void Render(Camera* camera) override;

  inline AtomicCommandQueue* update_rendering_queue();

  /** Used from any threads */
  inline std::shared_ptr<Session> current();

 private:
  AtomicCommandQueue update_rendering_queue_;

  DiscoveryBroadcasts discovery_broadcast_;
  std::shared_ptr<Session> current_session_;
  bool session_is_enabled_ = false;
  std::mutex current_session_mutex_;

  std::unique_ptr<GrpcServer> grpc_server_;
  std::shared_ptr<ILoop> loop_;
};

inline AtomicCommandQueue*
Remote::update_rendering_queue()
{
  return &update_rendering_queue_;
}

inline std::shared_ptr<Session>
Remote::current()
{
  return current_session_;
}

}  // namespace zen::remote::client
