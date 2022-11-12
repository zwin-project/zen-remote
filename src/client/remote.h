#pragma once

#include "client/session-manager.h"
#include "core/common.h"
#include "zen-remote/client/remote.h"

namespace zen::remote::client {

class GrpcServer;

class Remote : public IRemote {
 public:
  DISABLE_MOVE_AND_COPY(Remote);
  Remote() = delete;
  Remote(std::unique_ptr<ILoop> loop);

  void Start() override;

  /** Call only once before rendering for multiple cameras. */
  void UpdateScene() override;

  /**
   * Before calling this, bind a framebuffer, set a viewport, and clear the
   * framebuffer
   */
  void Render(Camera* camera) override;

  inline SessionManager* session_manager();

 private:
  std::unique_ptr<GrpcServer> grpc_server_;
  SessionManager session_manager_;
  std::shared_ptr<ILoop> loop_;
};

inline SessionManager*
Remote::session_manager()
{
  return &session_manager_;
}

}  // namespace zen::remote::client
