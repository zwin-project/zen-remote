#pragma once

#include "core/common.h"
#include "zen-remote/server/virtual-object.h"

namespace zen::remote::server {

class Remote;

class VirtualObject final : public IVirtualObject {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(std::shared_ptr<Remote>);
  ~VirtualObject();

  void Init();

  void Commit() override;

  uint64_t id() override;

 private:
  std::shared_ptr<Remote> remote_;
  uint64_t id_;
};

}  // namespace zen::remote::server
