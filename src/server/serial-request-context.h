#pragma once

#include "core/common.h"

namespace zen::remote::server {

class Remote;

class SerialRequestContext final : public grpc::ClientContext {
 public:
  DISABLE_MOVE_AND_COPY(SerialRequestContext);
  SerialRequestContext(const std::shared_ptr<Remote> &remote);
};

}  // namespace zen::remote::server
