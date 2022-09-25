#pragma once

#include <memory>

namespace zen::display_system::remote::client {

struct ISession {
  virtual ~ISession() = default;

  virtual bool Init() = 0;
};

std::unique_ptr<ISession> SessionCreate();

}  // namespace zen::display_system::remote::client
