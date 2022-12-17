#include "client/gl-texture.h"

#include "client/atomic-command-queue.h"
#include "core/logger.h"

namespace zen::remote::client {

GlTexture::DataCommand::DataCommand(enum DataCommandType type,
    union DataCommandArg arg, const std::string &data)
    : type(type), arg(arg), data(data)
{
}

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
}

void
GlTexture::Commit()
{
  if (pending_.data_commands.empty() && pending_.generate_mipmap_target == 0)
    return;

  std::list<DataCommand> data_commands;
  pending_.data_commands.swap(data_commands);

  auto command = CreateCommand(
      [data_commands = std::move(data_commands),
          generate_mipmap_target = pending_.generate_mipmap_target,
          rendering = rendering_](bool cancel) {
        if (cancel) {
          return;
        }

        if (rendering->texture_id == 0) {
          glGenTextures(1, &rendering->texture_id);
        }

        for (auto &command : data_commands) {
          switch (command.type) {
            case GlTexture::kImage2D: {
              auto args = command.arg.image_2d;
              glBindTexture(args.target, rendering->texture_id);
              glTexImage2D(args.target, args.level, args.internal_format,
                  args.width, args.height, args.border, args.format, args.type,
                  command.data.data());
              glBindTexture(args.target, 0);
              break;
            }
            case GlTexture::kSubImage2D: {
              auto args = command.arg.sub_image_2d;
              glBindTexture(args.target, rendering->texture_id);
              glTexSubImage2D(args.target, args.level, args.xoffset,
                  args.yoffset, args.width, args.height, args.format, args.type,
                  command.data.data());
              glBindTexture(args.target, 0);
              break;
            }
          }
        }

        // TODO: Should we preserve the order of data_commands and generating
        // mipmap command?
        if (generate_mipmap_target != 0) {
          glBindTexture(generate_mipmap_target, rendering->texture_id);
          glGenerateMipmap(generate_mipmap_target);
          glBindTexture(generate_mipmap_target, 0);
        }
      });

  update_rendering_queue_->Push(std::move(command));

  pending_.generate_mipmap_target = 0;
}

void
GlTexture::GlTexImage2D(uint32_t target, int32_t level, int32_t internal_format,
    uint32_t width, uint32_t height, int32_t border, uint32_t format,
    uint32_t type, const std::string &data)
{
  union DataCommandArg arg;
  arg.image_2d.target = target;
  arg.image_2d.level = level;
  arg.image_2d.internal_format = internal_format;
  arg.image_2d.width = width;
  arg.image_2d.height = height;
  arg.image_2d.border = border;
  arg.image_2d.format = format;
  arg.image_2d.type = type;

  pending_.data_commands.emplace_back(kImage2D, arg, data);
}

void
GlTexture::GlTexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
    int32_t yoffset, uint32_t width, uint32_t height, uint32_t format,
    uint32_t type, const std::string &data)
{
  union DataCommandArg arg;
  arg.sub_image_2d.target = target;
  arg.sub_image_2d.level = level;
  arg.sub_image_2d.xoffset = xoffset;
  arg.sub_image_2d.yoffset = yoffset;
  arg.sub_image_2d.width = width;
  arg.sub_image_2d.height = height;
  arg.sub_image_2d.format = format;
  arg.sub_image_2d.type = type;

  pending_.data_commands.emplace_back(kSubImage2D, arg, data);
}

void
GlTexture::GlGenerateMipmap(uint32_t target)
{
  pending_.generate_mipmap_target = target;
}

uint64_t
GlTexture::id()
{
  return id_;
}

}  // namespace zen::remote::client
