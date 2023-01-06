#pragma once

#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

/**
 * Channel should be owned only by the user. Channel is passed to zen-remote
 * objects as a shared_ptr, but the zen-remote objects never own the channel; if
 * the zen-remote objects have a reference to the channel, it is held as a
 * weak_ptr.
 */
struct IChannel {
  virtual ~IChannel() = default;

  virtual uint32_t GetBusyness() = 0;

  virtual bool wired() = 0;
};

std::shared_ptr<IChannel> CreateChannel(std::shared_ptr<ISession>& session);

}  // namespace zen::remote::server
