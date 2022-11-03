#pragma once

#include <zen-remote/loop.h>
#include <zen-remote/server/peer.h>
#include <zen-remote/signal.h>

#include <memory>

namespace zen::remote::server {

struct IPeerManager {
  virtual ~IPeerManager() = default;

  virtual std::shared_ptr<IPeer> Get(uint64_t peer_id) = 0;

  Signal<void(std::shared_ptr<IPeer> peer)> on_peer_discover;
  Signal<void(std::shared_ptr<IPeer> peer)> on_peer_lost;
  Signal<void(std::string message)> on_error;
  Signal<void(std::string message)> on_warn;
};

std::unique_ptr<IPeerManager> CreatePeerManager(std::unique_ptr<ILoop> loop);

}  // namespace zen::remote::server
