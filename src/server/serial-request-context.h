#pragma once

#include "core/common.h"

namespace zen::remote::server {

class Session;

class SerialRequestContext final : public grpc::ClientContext {
 public:
  DISABLE_MOVE_AND_COPY(SerialRequestContext);
  SerialRequestContext(Session* session);
};

}  // namespace zen::remote::server
