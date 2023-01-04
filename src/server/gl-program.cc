#include "server/gl-program.h"

#include "core/logger.h"
#include "gl-program.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/channel.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

GlProgram::GlProgram(std::shared_ptr<Channel> channel)
    : id_(channel->NewSerial(Channel::kResource)), channel_(std::move(channel))
{
}

void
GlProgram::Init()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlProgramService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<&GlProgramService::Stub::PrepareAsyncNew>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlProgram::New");
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
GlProgram::GlAttachShader(uint64_t shader_id)
{
  auto job =
      CreateJob([id = id_, channel_weak = channel_, shader_id](bool cancel) {
        auto channel = channel_weak.lock();
        if (cancel || !channel) {
          return;
        }

        auto context = std::unique_ptr<grpc::ClientContext>(
            new SerialRequestContext(channel));

        auto stub = GlProgramService::NewStub(channel->grpc_channel());

        auto caller = new AsyncGrpcCaller<
            &GlProgramService::Stub::PrepareAsyncGlAttachShader>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlProgram::GlAttachShader");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

        caller->request()->set_id(id);
        caller->request()->set_shader_id(shader_id);

        channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
      });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
GlProgram::GlLinkProgram()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlProgramService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlProgramService::Stub::PrepareAsyncGlLinkProgram>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlProgram::GlLinkProgram");
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

GlProgram::~GlProgram()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = GlProgramService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlProgramService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlProgram::Delete");
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
GlProgram::id()
{
  return id_;
}

std::unique_ptr<IGlProgram>
CreateGlProgram(std::shared_ptr<IChannel> channel)
{
  auto gl_program =
      std::make_unique<GlProgram>(std::dynamic_pointer_cast<Channel>(channel));

  gl_program->Init();

  return gl_program;
}

}  // namespace zen::remote::server
