#include "server/gl-vertex-array.h"

#include "core/logger.h"
#include "gl-vertex-array.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/channel.h"
#include "server/job.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

GlVertexArray::GlVertexArray(std::shared_ptr<Channel> channel)
    : id_(channel->NewSerial(Channel::kResource)), channel_(std::move(channel))
{
}

void
GlVertexArray::Init()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlVertexArrayService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlVertexArrayService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlVertexArray::New");
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
GlVertexArray::GlEnableVertexAttribArray(uint32_t index)
{
  auto job = CreateJob([id = id_, channel_weak = channel_, index](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlVertexArrayService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlVertexArrayService::Stub::PrepareAsyncGlEnableVertexAttribArray>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN(
                "Failed to call remote "
                "GlVertexArray::GlEnableVertexAttribArray");
            if (auto channel = channel_weak.lock())
              channel->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_index(index);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
GlVertexArray::GlDisableVertexAttribArray(uint32_t index)
{
  auto job = CreateJob([id = id_, channel_weak = channel_, index](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlVertexArrayService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlVertexArrayService::Stub::PrepareAsyncGlDisableVertexAttribArray>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN(
                "Failed to call remote "
                "GlVertexArray::GlDisableVertexAttribArray");
            if (auto channel = channel_weak.lock())
              channel->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_index(index);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
GlVertexArray::GlVertexAttribPointer(uint32_t index, int32_t size,
    uint32_t type, bool normalized, int32_t stride, uint64_t offset,
    uint64_t gl_buffer_id)
{
  auto job =
      CreateJob([id = id_, channel_weak = channel_, index, size, type,
                    normalized, stride, offset, gl_buffer_id](bool cancel) {
        auto channel = channel_weak.lock();
        if (cancel || !channel) {
          return;
        }

        auto context = std::unique_ptr<grpc::ClientContext>(
            new SerialRequestContext(channel));

        auto stub = GlVertexArrayService::NewStub(channel->grpc_channel());

        auto caller = new AsyncGrpcCaller<
            &GlVertexArrayService::Stub::PrepareAsyncGlVertexAttribPointer>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN(
                    "Failed to call remote "
                    "GlVertexArray::GlVertexAttribPointer");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

        caller->request()->set_id(id);
        caller->request()->set_index(index);
        caller->request()->set_size(size);
        caller->request()->set_type(type);
        caller->request()->set_normalized(normalized);
        caller->request()->set_stride(stride);
        caller->request()->set_offset(offset);
        caller->request()->set_gl_buffer_id(gl_buffer_id);

        channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
      });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

GlVertexArray::~GlVertexArray()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlVertexArrayService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlVertexArrayService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlVertexArray::Delete");
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
GlVertexArray::id()
{
  return id_;
}

std::unique_ptr<IGlVertexArray>
CreateGlVertexArray(std::shared_ptr<IChannel> channel)
{
  auto gl_buffer = std::make_unique<GlVertexArray>(
      std::dynamic_pointer_cast<Channel>(channel));
  gl_buffer->Init();

  return gl_buffer;
}

}  // namespace zen::remote::server
