#pragma once

#include <zen/display-system/remote/client.h>

namespace zen::display_system::remote::client {

class Session : public ISession {
 public:
  bool Init() override;
};

}  // namespace zen::display_system::remote::client
