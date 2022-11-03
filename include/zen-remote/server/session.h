#pragma once

#include <zen-remote/loop.h>
#include <zen-remote/server/peer.h>
#include <zen-remote/signal.h>

#include <memory>

namespace zen::remote::server {

struct ISession {
  virtual ~ISession() = default;

  virtual bool Connect(std::shared_ptr<IPeer> peer) = 0;

  Signal<void()> on_disconnect;
};

std::unique_ptr<ISession> CreateSession(std::unique_ptr<ILoop> loop);

}  // namespace zen::remote::server
