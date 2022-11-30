#pragma once

#include <zen-remote/loop.h>

#include <memory>

#include "camera.h"

namespace zen::remote::client {

struct IRemote {
  virtual ~IRemote() = default;

  virtual void StartGrpcServer() = 0;

  virtual void EnableSession() = 0;

  virtual void DisableSession() = 0;

  virtual void UpdateScene() = 0;  // deprecated

  virtual void Render(Camera *camera) = 0;  // deprecated
};

std::unique_ptr<IRemote> CreateRemote(std::unique_ptr<ILoop> loop);

}  // namespace zen::remote::client
