#include "client/gl-texture.h"

#include "client/atomic-command-queue.h"
#include "core/logger.h"

namespace zen::remote::client {

GlTexture::GlTexture(uint64_t id, AtomicCommandQueue *update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

GlTexture::~GlTexture()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {
    if (rendering->texture_id != 0) {
      glDeleteTextures(1, &rendering->texture_id);
    }
  });

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));

  free(pending_.data);
}

void
GlTexture::Commit()
{
  auto command = CreateCommand([rendering = rendering_](bool cancel) {
    if (cancel) {
      return;
    }

    if (rendering->texture_id == 0) {
      glGenTextures(1, &rendering->texture_id);
    }
  });
  update_rendering_queue_->Push(std::move(command));

  if (pending_.damaged) {
    pending_.damaged = false;

    // ownership of pending_.data moves
    auto command = CreateCommand(
        [image_args = pending_.args, type = pending_.type, data = pending_.data,
            rendering = rendering_](bool cancel) {
          if (cancel) {
            free(data);
            return;
          }

          switch (type) {
            case ImageType::kNone:
              break;
            case ImageType::k2D: {
              auto args = image_args.image_2d;
              glBindTexture(args.target, rendering->texture_id);
              glTexImage2D(args.target, args.level, args.internal_format,
                  args.width, args.height, args.border, args.format, args.type,
                  data);

              // FIXME: Client should be able to specify the parameters below
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

              glBindTexture(args.target, 0);
              break;
            }
          }
          free(data);
        });
    update_rendering_queue_->Push(std::move(command));

    pending_.data = NULL;
    pending_.alloc = 0;
    pending_.size = 0;
  }
}

void
GlTexture::GlTexImage2D(uint32_t target, int32_t level, int32_t internal_format,
    uint32_t width, uint32_t height, int32_t border, uint32_t format,
    uint32_t type, std::size_t size, const void *data)
{
  if (size > pending_.alloc) {
    pending_.data = realloc(pending_.data, size);
    pending_.alloc = size;
  }

  memcpy(pending_.data, data, size);
  pending_.size = size;

  pending_.type = ImageType::k2D;
  pending_.args.image_2d.target = target;
  pending_.args.image_2d.level = level;
  pending_.args.image_2d.internal_format = internal_format;
  pending_.args.image_2d.width = width;
  pending_.args.image_2d.height = height;
  pending_.args.image_2d.border = border;
  pending_.args.image_2d.format = format;
  pending_.args.image_2d.type = type;

  pending_.damaged = true;
}

uint64_t
GlTexture::id()
{
  return id_;
}

}  // namespace zen::remote::client
