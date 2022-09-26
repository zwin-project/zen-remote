#pragma once

#include "zen/display-system/remote/server.h"

namespace zen::display_system::remote::server {

class Session : public ISession {
 public:
  bool Init() override;
};

}  // namespace zen::display_system::remote::server
