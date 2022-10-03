#pragma once

#include "core/loop.h"
#include <memory>

namespace zen::display_system::remote::server {

struct IRemote {
  virtual ~IRemote() = default;

  virtual void Start() = 0;
};

std::unique_ptr<IRemote> CreateRemote(std::unique_ptr<ILoop> loop);

}  // namespace zen::display_system::remote::server
