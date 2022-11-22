#pragma once

#include "client/gl-buffer.h"
#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

enum class TextureTarget {
  kNone,
  kImage2D,
};

class GlTexture final : public IResource {
  union ImageArgs {
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
  };

  struct RenderingState {
    GLuint texture_id = 0;
    TextureTarget target;
    ImageArgs args;
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
      uint32_t type, size_t size, const void *data);

  uint64_t id() override;

  /** Used in the rendering thread */
  inline GLuint texture_id();

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    void *data = NULL;
    size_t size = 0;
    size_t alloc = 0;

    TextureTarget target = TextureTarget::kNone;
    ImageArgs args;

    /* args, target, data are expected to be changed at the same time */
    bool damaged = false;
  } pending_;

  std::shared_ptr<RenderingState> rendering_;
};

inline GLuint
GlTexture::texture_id()
{
  return rendering_->texture_id;
}
}  // namespace zen::remote::client
