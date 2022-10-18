#include "client/remote.h"

#include "core/common.h"
#include "core/connection/peer.h"
#include "core/logger.h"
#include "zen-remote/client/remote.h"

namespace zen::remote::client {

void
Remote::Start()
{
  peer_ = std::make_unique<connection::Peer>(
      connection::Peer::Target::kServer, context_);
  peer_->signals.discoverd.connect([this] {
    std::string server_ip = peer_->endpoint().address().to_string();
    LOG_INFO("Server Found: %s", server_ip.c_str());
  });
  peer_->StartDiscover();

  grpc_server_ = std::make_unique<GrpcServer>("0.0.0.0", kGrpcPort, pool_);
  grpc_server_->Start();
}

void
Remote::Stop()
{
  // TODO:
}

std::shared_ptr<IResourcePool>
Remote::pool()
{
  return pool_;
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::remote::client
