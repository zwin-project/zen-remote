#include "server/rendering-unit.h"

#include "core/logger.h"
#include "rendering-unit.grpc.pb.h"

namespace zen::remote::server {

void
RenderingUnit::Init()
{
  uint64_t id = id_;
  auto remote = remote_;

  auto job = std::make_unique<Job>([id, remote](bool cancel) {
    if (cancel) return;

    auto& peer = remote->peer();

    auto host_port = peer->endpoint().address().to_string() + ":" +
                     std::to_string(kGrpcPort);

    // TODO: share the channel across jobs
    auto channel =
        grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());

    auto stub = RenderingUnitService::NewStub(channel);

    NewObjectRequest request;
    EmptyResponse response;
    grpc::ClientContext context;

    request.set_id(id);

    auto status = stub->New(&context, request, &response);
    if (!status.ok()) {
      LOG_WARN("Failed to create a new rendering unit");
    }
  });

  remote_->job_queue()->Push(std::move(job));
}

RenderingUnit::~RenderingUnit()
{
  uint64_t id = id_;
  auto remote = remote_;

  auto job = std::make_unique<Job>([id, remote](bool cancel) {
    if (cancel) return;

    auto& peer = remote->peer();

    auto host_port = peer->endpoint().address().to_string() + ":" +
                     std::to_string(kGrpcPort);

    auto channel =
        grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());

    auto stub = RenderingUnitService::NewStub(channel);

    DeleteObjectRequest request;
    EmptyResponse response;
    grpc::ClientContext context;

    request.set_id(id);

    auto status = stub->Delete(&context, request, &response);
    if (!status.ok()) {
      LOG_WARN("Failed to destroy a rendering unit");
    }
  });

  remote_->job_queue()->Push(std::move(job));
}

std::unique_ptr<IRenderingUnit>
CreateRenderingUnit(std::shared_ptr<IRemote> remote)
{
  auto rendering_unit = std::make_unique<RenderingUnit>(
      std::dynamic_pointer_cast<Remote>(remote));

  rendering_unit->Init();

  return rendering_unit;
}

}  // namespace zen::remote::server
