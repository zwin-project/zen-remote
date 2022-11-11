#pragma once

#include <zen-remote/loop.h>
#include <zen-remote/server/peer.h>
#include <zen-remote/signal.h>

#include <memory>

namespace zen::remote::server {

/**
 * Session is passed to zen-remote objects as a shared_ptr, but the zen-remote
 * objects never own the session; if the zen-remote objests have a reference to
 * the session, it is held as a weak_ptr.
 */
struct ISession {
  virtual ~ISession() = default;

  virtual bool Connect(std::shared_ptr<IPeer> peer) = 0;

  Signal<void()> on_disconnect;
};

std::unique_ptr<ISession> CreateSession(std::unique_ptr<ILoop> loop);

}  // namespace zen::remote::server
