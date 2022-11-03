#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-base-technique.h"

namespace zen::remote::server {

class Remote;

class GlBaseTechnique final : public IGlBaseTechnique {
 public:
  DISABLE_MOVE_AND_COPY(GlBaseTechnique);
  GlBaseTechnique() = delete;
  GlBaseTechnique(std::shared_ptr<Remote> remote);
  ~GlBaseTechnique();

  void Init(uint64_t rendering_unit_id);

  void GlDrawArrays(uint32_t mode, int32_t first, uint32_t count) override;

  uint64_t id() override;

 private:
  std::shared_ptr<Remote> remote_;
  uint64_t id_;
};

}  // namespace zen::remote::server
