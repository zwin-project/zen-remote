#include "server/gl-shader.h"

#include "core/logger.h"
#include "gl-shader.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/channel.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

GlShader::GlShader(std::shared_ptr<Channel> channel)
    : id_(channel->NewSerial(Channel::kResource)), channel_(std::move(channel))
{
}

void
GlShader::Init(std::string source, uint32_t type)
{
  auto job = CreateJob([id = id_, channel_weak = channel_,
                           source = std::move(source), type](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlShaderService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<&GlShaderService::Stub::PrepareAsyncNew>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlShader::New");
            if (auto channel = channel_weak.lock())
              channel->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_source(std::move(source));
    caller->request()->set_type(type);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

GlShader::~GlShader()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlShaderService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlShaderService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlShader::Delete");
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
GlShader::id()
{
  return id_;
}

std::unique_ptr<IGlShader>
CreateGlShader(
    std::shared_ptr<IChannel> channel, std::string source, uint32_t type)
{
  auto gl_shader =
      std::make_unique<GlShader>(std::dynamic_pointer_cast<Channel>(channel));

  gl_shader->Init(std::move(source), type);

  return gl_shader;
}

}  // namespace zen::remote::server
