#include "server/gl-buffer.h"

#include "core/logger.h"
#include "gl-buffer.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/buffer.h"
#include "server/channel.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

GlBuffer::GlBuffer(std::shared_ptr<Channel> channel)
    : id_(channel->NewSerial(Channel::kResource)), channel_(std::move(channel))
{
}

void
GlBuffer::Init()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlBufferService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<&GlBufferService::Stub::PrepareAsyncNew>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBuffer::New");
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
GlBuffer::GlBufferData(std::unique_ptr<IBuffer> buffer, uint32_t target,
    size_t size, uint32_t usage)
{
  auto job = CreateJob([id = id_, channel_weak = channel_,
                           buffer = std::move(buffer), target, size,
                           usage](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlBufferService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBufferService::Stub::PrepareAsyncGlBufferData>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBuffer::GlBufferData");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);
    caller->request()->set_target(target);
    caller->request()->set_usage(usage);

    void* data = buffer->begin_access();
    caller->request()->set_data(data, size);
    buffer->end_access();

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

GlBuffer::~GlBuffer()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlBufferService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBufferService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBuffer::Delete");
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
GlBuffer::id()
{
  return id_;
}

std::unique_ptr<IGlBuffer>
CreateGlBuffer(std::shared_ptr<IChannel> channel)
{
  auto gl_buffer =
      std::make_unique<GlBuffer>(std::dynamic_pointer_cast<Channel>(channel));

  gl_buffer->Init();

  return gl_buffer;
}

}  // namespace zen::remote::server
