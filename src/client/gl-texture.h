#pragma once

#include "client/gl-buffer.h"
#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

class GlTexture final : public IResource {
  enum DataCommandType {
    kImage2D = 0,
    kSubImage2D,
  };

  union DataCommandArg {
    struct {
      uint32_t target;
      int32_t level;
      int32_t internal_format;
      uint32_t width;
      uint32_t height;
      int32_t border;
      uint32_t format;
      uint32_t type;
    } image_2d;

    struct {
      uint32_t target;
      int32_t level;
      int32_t xoffset;
      int32_t yoffset;
      uint32_t width;
      uint32_t height;
      uint32_t format;
      uint32_t type;
    } sub_image_2d;
  };

  struct DataCommand {
    DataCommand(const DataCommand &) = delete;
    DataCommand &operator=(const DataCommand &) = delete;
    DataCommand(enum DataCommandType type, union DataCommandArg arg,
        const std::string &data);

    enum DataCommandType type;
    union DataCommandArg arg;
    const std::string data;
  };

  struct RenderingState {
    GLuint texture_id = 0;
  };

 public:
  DISABLE_MOVE_AND_COPY(GlTexture);
  GlTexture() = delete;
  GlTexture(uint64_t id, AtomicCommandQueue *update_rendering_queue);
  ~GlTexture();

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void GlTexImage2D(uint32_t target, int32_t level, int32_t internal_format,
      uint32_t width, uint32_t height, int32_t border, uint32_t format,
      uint32_t type, const std::string &data);

  /** Used in the update thread */
  void GlTexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
      int32_t yoffset, uint32_t width, uint32_t height, uint32_t format,
      uint32_t type, const std::string &data);

  uint64_t id() override;

  /** Used in the rendering thread */
  inline GLuint texture_id();

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    std::list<DataCommand> data_commands;  // push to back, apply from front
  } pending_;

  std::shared_ptr<RenderingState> rendering_;
};

inline GLuint
GlTexture::texture_id()
{
  return rendering_->texture_id;
}
}  // namespace zen::remote::client
