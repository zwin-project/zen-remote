#pragma once

#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IGlProgram {
  virtual ~IGlProgram() = default;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlProgram> CreateGlProgram(std::shared_ptr<ISession> session);

}  // namespace zen::remote::server
