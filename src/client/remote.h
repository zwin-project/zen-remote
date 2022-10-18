#pragma once

#include "client/grpc-server.h"
#include "client/resource-pool.h"
#include "core/connection/peer.h"
#include "core/context.h"
#include "zen-remote/client/remote.h"

namespace zen::remote::client {

class Remote : public IRemote {
 public:
  Remote(std::unique_ptr<ILoop> loop)
      : context_(std::make_unique<Context>(std::move(loop))),
        pool_(std::make_shared<ResourcePool>())
  {
  }

  void Start() override;

  void Stop() override;

  std::shared_ptr<IResourcePool> pool() override;

 private:
  std::unique_ptr<connection::Peer> peer_;
  std::shared_ptr<Context> context_;
  std::unique_ptr<GrpcServer> grpc_server_;
  std::shared_ptr<ResourcePool> pool_;
};

}  // namespace zen::remote::client
