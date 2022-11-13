#pragma once

#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IGlVertexArray {
  virtual ~IGlVertexArray() = default;
};

std::unique_ptr<IGlVertexArray> CreateGlVertexArray(
    std::shared_ptr<ISession> session);

}  // namespace zen::remote::server
