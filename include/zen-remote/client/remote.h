#pragma once

#include <zen-remote/client/resource-pool.h>
#include <zen-remote/loop.h>

#include <memory>

namespace zen::remote::client {

struct IRemote {
  virtual ~IRemote() = default;

  virtual void Start() = 0;

  virtual void Stop() = 0;

  virtual IResourcePool* pool() = 0;
};

std::unique_ptr<IRemote> CreateRemote(std::unique_ptr<ILoop> loop);

}  // namespace zen::remote::client
