#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-base-technique.h"

namespace zen::remote::server {

class Session;

class GlBaseTechnique final : public IGlBaseTechnique {
 public:
  DISABLE_MOVE_AND_COPY(GlBaseTechnique);
  GlBaseTechnique() = delete;
  GlBaseTechnique(std::shared_ptr<Session> session);
  ~GlBaseTechnique();

  void Init(uint64_t rendering_unit_id);

  void BindProgram(uint64_t program_id) override;

  void BindVertexArray(uint64_t vertex_array_id) override;

  void GlDrawArrays(uint32_t mode, int32_t first, uint32_t count) override;

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Session> session_;
};

}  // namespace zen::remote::server
