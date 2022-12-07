#pragma once

#include <zen-remote/server/session.h>

#include <climits>
#include <memory>

namespace zen::remote::server {

struct IGlBaseTechnique {
  static_assert(CHAR_BIT * sizeof(float) == 32);

  virtual ~IGlBaseTechnique() = default;

  virtual void BindProgram(uint64_t program_id) = 0;

  virtual void BindVertexArray(uint64_t vertex_array_id) = 0;

  virtual void BindTexture(uint32_t binding, std::string name,
      uint64_t texture_id, uint32_t target, uint64_t sampler_id) = 0;

  /**
   * @param value must be larger than or equeal to (32 * size * count) bits
   */
  virtual void GlUniformVector(uint32_t location, std::string name,
      uint32_t size, uint32_t count, int32_t* value) = 0;

  /**
   * @param value must be larger than or equeal to (32 * size * count) bits
   */
  virtual void GlUniformVector(uint32_t location, std::string name,
      uint32_t size, uint32_t count, uint32_t* value) = 0;

  /**
   * @param value must be larger than or equeal to (32 * size * count) bits
   */
  virtual void GlUniformVector(uint32_t location, std::string name,
      uint32_t size, uint32_t count, float* value) = 0;

  /**
   * @param value must be larger than or equeal to (32 * col * row * count) bits
   */
  virtual void GlUniformMatrix(uint32_t location, std::string name,
      uint32_t col, uint32_t row, uint32_t count, bool transpose,
      float* value) = 0;

  virtual void GlDrawArrays(uint32_t mode, int32_t first, uint32_t count) = 0;

  virtual void GlDrawElements(uint32_t mode, uint32_t count, uint32_t type,
      uint64_t offset, uint64_t element_array_buffer_id) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlBaseTechnique> CreateGlBaseTechnique(
    std::shared_ptr<ISession> session, uint64_t rendering_unit_id);

}  // namespace zen::remote::server
