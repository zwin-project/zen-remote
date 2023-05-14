#pragma once

#include "core/common.h"
#include "zen-remote/server/virtual-object.h"

namespace zen::remote::server {

class Channel;

class VirtualObject final : public IVirtualObject {
 public:
  DISABLE_MOVE_AND_COPY(VirtualObject);
  VirtualObject() = delete;
  VirtualObject(std::shared_ptr<Channel> channel);
  ~VirtualObject();

  void Init();

  void Commit() override;

  void Move(float position[3], float quaternion[4]) override;

  void ChangeVisibility(bool visible) override;

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Channel> channel_;
};

}  // namespace zen::remote::server
