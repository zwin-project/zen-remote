#include "client/remote.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "zen-remote/remote.h"

namespace zen::remote {

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

void
Remote::Stop()
{
  // TODO:
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::remote
