#pragma once

#include <zen-remote/server/session.h>

#include <memory>

namespace zen::remote::server {

struct IGlBaseTechnique {
  virtual ~IGlBaseTechnique() = default;

  virtual void BindProgram(uint64_t program_id) = 0;

  virtual void BindVertexArray(uint64_t vertex_array_id) = 0;

  virtual void GlDrawArrays(uint32_t mode, int32_t first, uint32_t count) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlBaseTechnique> CreateGlBaseTechnique(
    std::shared_ptr<ISession> session, uint64_t rendering_unit_id);

}  // namespace zen::remote::server
