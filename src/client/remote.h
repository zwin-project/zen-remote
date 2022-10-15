#pragma once

#include "client/grpc-server.h"
#include "core/connection/peer.h"
#include "core/context.h"
#include "zen-remote/client/remote.h"

namespace zen::remote::client {

class Remote : public IRemote {
 public:
  Remote(std::unique_ptr<ILoop> loop)
      : context_(std::make_unique<Context>(std::move(loop)))
  {
  }

  void Start() override;

  void Stop() override;

 private:
  std::unique_ptr<connection::Peer> peer_;
  std::shared_ptr<Context> context_;
  std::unique_ptr<GrpcServer> grpc_server_;
};

}  // namespace zen::remote::client
