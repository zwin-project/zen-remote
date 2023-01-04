#pragma once

#include <zen-remote/server/channel.h>

#include <memory>

namespace zen::remote::server {

struct IGlShader {
  virtual ~IGlShader() = default;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlShader> CreateGlShader(
    std::shared_ptr<IChannel> channel, std::string source, uint32_t type);

}  // namespace zen::remote::server
