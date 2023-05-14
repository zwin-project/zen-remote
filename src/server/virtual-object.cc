#include "server/virtual-object.h"

#include "core/logger.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/channel.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "virtual-object.grpc.pb.h"

namespace zen::remote::server {

VirtualObject::VirtualObject(std::shared_ptr<Channel> channel)
    : id_(channel->NewSerial(Channel::kResource)), channel_(std::move(channel))
{
}

void
VirtualObject::Init()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = VirtualObjectService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&VirtualObjectService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::New");
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
VirtualObject::Commit()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = VirtualObjectService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&VirtualObjectService::Stub::PrepareAsyncCommit>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::Commit");
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
VirtualObject::Move(float position[3], float quaternion[4])
{
  std::string position_container(sizeof(float[3]), ' ');
  std::memcpy(position_container.data(), position, sizeof(float[3]));
  std::string quaternion_container(sizeof(float[4]), ' ');
  std::memcpy(quaternion_container.data(), quaternion, sizeof(float[4]));

  auto job = CreateJob([id = id_, channel_weak = channel_,
                           position = std::move(position_container),
                           quaternion = std::move(quaternion_container)](
                           bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = VirtualObjectService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&VirtualObjectService::Stub::PrepareAsyncMove>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::Move");
                if (auto channel = channel_weak.lock())
                  channel->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);
    caller->request()->set_position(std::move(position));
    caller->request()->set_quaternion(std::move(quaternion));

    channel->PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  if (auto channel = channel_.lock()) {
    channel->PushJob(std::move(job));
  }
}

void
VirtualObject::ChangeVisibility(bool visible)
{
  auto job = CreateJob([id = id_, channel_weak = channel_, visible](
                           bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = VirtualObjectService::NewStub(channel->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &VirtualObjectService::Stub::PrepareAsyncChangeVisibility>(
        std::move(stub), std::move(context),
        [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote VirtualObject::ChangeVisibility");
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

VirtualObject::~VirtualObject()
{
  auto job = CreateJob([id = id_, channel_weak = channel_](bool cancel) {
    auto channel = channel_weak.lock();
    if (cancel || !channel) {
      return;
    }

    auto context =
        std::unique_ptr<grpc::ClientContext>(new SerialRequestContext(channel));

    auto stub = VirtualObjectService::NewStub(channel->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&VirtualObjectService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [channel_weak](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote VirtualObject::Delete");
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
VirtualObject::id()
{
  return id_;
}

std::unique_ptr<IVirtualObject>
CreateVirtualObject(std::shared_ptr<IChannel> channel)
{
  auto virtual_object = std::make_unique<VirtualObject>(
      std::dynamic_pointer_cast<Channel>(channel));

  virtual_object->Init();

  return virtual_object;
}

}  // namespace zen::remote::server
