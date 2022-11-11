#pragma once

#include "core/common.h"
#include "zen-remote/server/virtual-object.h"

namespace zen::remote::server {

class Session;

class VirtualObject final : public IVirtualObject {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(std::shared_ptr<Session> session);
  ~VirtualObject();

  void Init();

  void Commit() override;

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Session> session_;
};

}  // namespace zen::remote::server
