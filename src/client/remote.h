#pragma once

#include "client/resource-pool.h"
#include "core/common.h"
#include "core/context.h"
#include "zen-remote/client/remote.h"

namespace zen::remote::connection {
class Peer;
}

namespace zen::remote::client {

class GrpcServer;

class Remote : public IRemote {
 public:
  DISABLE_MOVE_AND_COPY(Remote);
  Remote() = delete;
  Remote(std::unique_ptr<ILoop> loop);

  void Start() override;

  void Stop() override;

  /** Call only once before rendering for multiple cameras. */
  void UpdateScene() override;

  /**
   * Before calling this, bind a framebuffer, set a viewport, and clear the
   * framebuffer
   */
  void Render(Camera *camera) override;

 private:
  std::shared_ptr<Context> context_;
  std::unique_ptr<connection::Peer> peer_;
  std::unique_ptr<GrpcServer> grpc_server_;
  ResourcePool pool_;
};

}  // namespace zen::remote::client
