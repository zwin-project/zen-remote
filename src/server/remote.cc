#include "server/remote.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "zen-remote/remote.h"

namespace zen::remote {

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
