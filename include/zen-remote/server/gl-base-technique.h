#pragma once

#include <zen-remote/server/buffer.h>
#include <zen-remote/server/remote.h>

#include <memory>

namespace zen::remote::server {

struct IGlBaseTechnique {
  virtual ~IGlBaseTechnique() = default;

  virtual void GlDrawArrays(uint32_t mode, int32_t first, uint32_t count) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlBaseTechnique> CreateGlBaseTechnique(
    std::shared_ptr<IRemote> remote, uint64_t rendering_unit_id);

}  // namespace zen::remote::server
