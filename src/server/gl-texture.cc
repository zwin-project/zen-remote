#include "server/gl-texture.h"

#include <GLES3/gl32.h>

#include "core/logger.h"
#include "gl-texture.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/buffer.h"
#include "server/channel.h"
#include "server/job.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

namespace {

// See "8.4.2 Transfer of Pixel Rectangles" of the OpenGL ES 3.2 spec
size_t
CalcTextureSize(uint32_t width, uint32_t height, uint32_t format, uint32_t type)
{
  size_t external_bytes_per_pixel = 0;
  bool take_format_into_account = true;

  switch (type) {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
      external_bytes_per_pixel = 1;
      break;

    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
      take_format_into_account = false;
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_HALF_FLOAT:
      external_bytes_per_pixel = 2;
      break;

    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_UNSIGNED_INT_24_8:
      take_format_into_account = false;
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
      external_bytes_per_pixel = 4;
      break;

    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
      take_format_into_account = false;
      external_bytes_per_pixel = 8;
      break;
  }

  if (take_format_into_account) {
    switch (format) {
      case GL_RED:
      case GL_RED_INTEGER:
      case GL_DEPTH_COMPONENT:
      case GL_ALPHA:
      case GL_STENCIL_INDEX:
      case GL_LUMINANCE:
        external_bytes_per_pixel *= 1;
        break;
      case GL_RG:
      case GL_RG_INTEGER:
      case GL_DEPTH_STENCIL:
      case GL_LUMINANCE_ALPHA:
        external_bytes_per_pixel *= 2;
        break;
      case GL_RGB:
      case GL_RGB_INTEGER:
        external_bytes_per_pixel *= 3;
        break;
      case GL_RGBA:
      case GL_RGBA_INTEGER:
        external_bytes_per_pixel *= 4;
        break;
    }
  }

  return external_bytes_per_pixel * width * height;
}

}  // namespace

GlTexture::GlTexture(std::shared_ptr<Channel> channel)
    : id_(channel->NewSerial(Channel::kResource)), channel_(std::move(channel))
{
}

void
GlTexture::Init()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlTextureService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<&GlTextureService::Stub::PrepareAsyncNew>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlTexture::New");
            if (auto channel = channel_weak.lock())
              channel->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
GlTexture::GlTexImage2D(uint32_t target, int32_t level, int32_t internal_format,
    uint32_t width, uint32_t height, int32_t border, uint32_t format,
    uint32_t type, std::unique_ptr<IBuffer> buffer)
{
  auto job = CreateJob([id = id_, channel_weak = channel_, target, level,
                           internal_format, width, height, border, format, type,
                           buffer = std::move(buffer)](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));
    if ((channel->session_characteristics() & Session::kWired) == 0) {
      context->set_compression_algorithm(GRPC_COMPRESS_DEFLATE);
    }

    auto stub = GlTextureService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlTextureService::Stub::PrepareAsyncGlTexImage2D>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlTexture::GlTexImage2D");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

    size_t size = CalcTextureSize(width, height, format, type);

    caller->request()->set_id(id);
    caller->request()->set_target(target);
    caller->request()->set_level(level);
    caller->request()->set_internal_format(internal_format);
    caller->request()->set_width(width);
    caller->request()->set_height(height);
    caller->request()->set_border(border);
    caller->request()->set_format(format);
    caller->request()->set_type(type);
    caller->request()->set_data(buffer->data(), size);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
GlTexture::GlTexSubImage2D(uint32_t target, int32_t level, int32_t xoffset,
    int32_t yoffset, uint32_t width, uint32_t height, uint32_t format,
    uint32_t type, std::unique_ptr<IBuffer> buffer)
{
  auto job = CreateJob([id = id_, channel_weak = channel_, target, level,
                           xoffset, yoffset, width, height, format, type,
                           buffer = std::move(buffer)](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));
    if ((channel->session_characteristics() & Session::kWired) == 0) {
      context->set_compression_algorithm(GRPC_COMPRESS_DEFLATE);
    }

    auto stub = GlTextureService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlTextureService::Stub::PrepareAsyncGlTexSubImage2D>(std::move(stub),
        std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlTexture::GlTexSubImage2D");
            if (auto channel = channel_weak.lock())
              channel->NotifyDisconnection();
          }
        });

    size_t size = CalcTextureSize(width, height, format, type);

    caller->request()->set_id(id);
    caller->request()->set_target(target);
    caller->request()->set_level(level);
    caller->request()->set_xoffset(xoffset);
    caller->request()->set_yoffset(yoffset);
    caller->request()->set_width(width);
    caller->request()->set_height(height);
    caller->request()->set_format(format);
    caller->request()->set_type(type);
    caller->request()->set_data(buffer->data(), size);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
GlTexture::GlGenerateMipmap(uint32_t target)
{
  auto job =
      CreateJob([id = id_, channel_weak = channel_, target](bool cancel) {
        auto channel = channel_weak.lock();
        if (cancel || !channel) {
          return;
        }

        auto context = std::unique_ptr<grpc::ClientContext>(
            new SerialRequestContext(channel));

        auto stub = GlTextureService::NewStub(channel->grpc_channel());

        auto caller = new AsyncGrpcCaller<
            &GlTextureService::Stub::PrepareAsyncGlGenerateMipmap>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlTexture::GlGenerateMipmap");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

        caller->request()->set_id(id);
        caller->request()->set_target(target);

        channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
      });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

GlTexture::~GlTexture()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlTextureService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlTextureService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlTexture::Delete");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

uint64_t
GlTexture::id()
{
  return id_;
}

std::unique_ptr<IGlTexture>
CreateGlTexture(std::shared_ptr<IChannel> channel)
{
  auto gl_texture =
      std::make_unique<GlTexture>(std::dynamic_pointer_cast<Channel>(channel));
  gl_texture->Init();

  return gl_texture;
}

}  // namespace zen::remote::server
