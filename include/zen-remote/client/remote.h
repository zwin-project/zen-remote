#pragma once

#include <zen-remote/loop.h>

#include <memory>

#include "camera.h"

namespace zen::remote::client {

struct IRemote {
  virtual ~IRemote() = default;

  virtual void Start() = 0;

  virtual void Stop() = 0;

  virtual void UpdateScene() = 0;

  virtual void Render(Camera *camera) = 0;
};

std::unique_ptr<IRemote> CreateRemote(std::unique_ptr<ILoop> loop);

}  // namespace zen::remote::client
