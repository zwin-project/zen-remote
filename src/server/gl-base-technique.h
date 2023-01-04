#pragma once

#include "core/common.h"
#include "core/types.h"
#include "zen-remote/server/gl-base-technique.h"

namespace zen::remote::server {

class Channel;

class GlBaseTechnique final : public IGlBaseTechnique {
 public:
  DISABLE_MOVE_AND_COPY(GlBaseTechnique);
  GlBaseTechnique() = delete;
  GlBaseTechnique(std::shared_ptr<Channel> channel);
  ~GlBaseTechnique();

  void Init(uint64_t rendering_unit_id);

  void BindProgram(uint64_t program_id) override;

  void BindVertexArray(uint64_t vertex_array_id) override;

  void BindTexture(uint32_t binding, std::string name, uint64_t texture_id,
      uint32_t target, uint64_t sampler_id) override;

  void GlUniformVector(uint32_t location, std::string name, uint32_t size,
      uint32_t count, int32_t* value) override;

  void GlUniformVector(uint32_t location, std::string name, uint32_t size,
      uint32_t count, uint32_t* value) override;

  void GlUniformVector(uint32_t location, std::string name, uint32_t size,
      uint32_t count, float* value) override;

  void GlUniformMatrix(uint32_t location, std::string name, uint32_t col,
      uint32_t row, uint32_t count, bool transpose, float* value) override;

  void GlDrawArrays(uint32_t mode, int32_t first, uint32_t count) override;

  void GlDrawElements(uint32_t mode, uint32_t count, uint32_t type,
      uint64_t offset, uint64_t element_array_buffer_id) override;

  uint64_t id() override;

 private:
  void GlUniform(uint32_t location, std::string name, UniformVariableType type,
      uint32_t col, uint32_t row, uint32_t count, bool transpose, void* value);

  uint64_t id_;
  std::weak_ptr<Channel> channel_;
};

}  // namespace zen::remote::server
