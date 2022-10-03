#include "pch.h"

#include "client/remote.h"
#include "core/connection/peer.h"
#include "core/logger.h"
#include "zen/display-system/remote/client.h"

namespace zen::display_system::remote::client {

void
Remote::Start()
{
  peer_ = std::make_unique<connection::Peer>(
      connection::Peer::Target::kServer, context_);
  peer_->signals.discoverd.connect([this] {
    LOG_INFO(
        "Server Found: %s", peer_->endpoint().address().to_string().c_str());
  });
  peer_->StartDiscover();
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::display_system::remote::client
