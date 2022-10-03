#include "pch.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "server/remote.h"
#include "zen/display-system/remote/server.h"

namespace zen::display_system::remote::server {

namespace ip = boost::asio::ip;

void
Remote::Start()
{
  peer_ = std::make_unique<connection::Peer>(
      connection::Peer::Target::kClient, context_);
  peer_->signals.discoverd.connect([this] {
    LOG_INFO(
        "Client Found: %s", peer_->endpoint().address().to_string().c_str());
  });
  peer_->StartDiscover();
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::display_system::remote::server
