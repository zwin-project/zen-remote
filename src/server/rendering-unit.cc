#include "server/rendering-unit.h"

#include "core/logger.h"
#include "rendering-unit.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/channel.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

RenderingUnit::RenderingUnit(std::shared_ptr<Channel> channel)
    : id_(channel->NewSerial(Channel::kResource)), channel_(std::move(channel))
{
}

void
RenderingUnit::Init(uint64_t virtual_object_id)
{
  auto job = CreateJob([id = id_, channel_weak = channel_, virtual_object_id](
                           bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = RenderingUnitService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&RenderingUnitService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote RenderingUnit::New");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);
    caller->request()->set_virtual_object_id(virtual_object_id);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
RenderingUnit::ChangeVisibility(bool visible)
{
  auto job = CreateJob([id = id_, channel_weak = channel_, visible](
                           bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = RenderingUnitService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &RenderingUnitService::Stub::PrepareAsyncChangeVisibility>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote RenderingUnit::ChangeVisibility");
            if (auto channel = channel_weak.lock()) {
              channel->NotifyDisconnection();
            }
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_visible(visible);

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

RenderingUnit::~RenderingUnit()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = RenderingUnitService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&RenderingUnitService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote RenderingUnit::Delete");
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
RenderingUnit::id()
{
  return id_;
}

std::unique_ptr<IRenderingUnit>
CreateRenderingUnit(
    std::shared_ptr<IChannel> channel, uint64_t virtual_object_id)
{
  auto rendering_unit = std::make_unique<RenderingUnit>(
      std::dynamic_pointer_cast<Channel>(channel));

  rendering_unit->Init(virtual_object_id);

  return rendering_unit;
}

}  // namespace zen::remote::server
