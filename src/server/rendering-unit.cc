#include "server/rendering-unit.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "rendering-unit.grpc.pb.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/remote.h"

namespace zen::remote::server {

RenderingUnit::RenderingUnit(std::shared_ptr<Remote> remote)
    : remote_(std::move(remote)),
      id_(remote_->NewSerial(Remote::SerialType::kResource))
{
}

void
RenderingUnit::Init(uint64_t virtual_object_id)
{
  auto job = std::make_unique<Job>(
      [id = id_, virtual_object_id, remote = remote_](bool cancel) {
        if (cancel) return;

        auto channel = remote->peer()->grpc_channel();

        auto stub = RenderingUnitService::NewStub(channel);

        NewRenderingUnitRequest request;
        EmptyResponse response;
        grpc::ClientContext context;

        request.set_id(id);
        request.set_virtual_object_id(virtual_object_id);

        auto status = stub->New(&context, request, &response);
        if (!status.ok()) {
          LOG_WARN("Failed to create a new rendering unit");
        }
      });

  remote_->job_queue()->Push(std::move(job));
}

void
RenderingUnit::GlEnableVertexAttribArray(uint32_t index)
{
  auto job =
      std::make_unique<Job>([id = id_, index, remote = remote_](bool cancel) {
        if (cancel) return;

        auto channel = remote->peer()->grpc_channel();

        auto stub = RenderingUnitService::NewStub(channel);

        GlEnableVertexAttribArrayRequest request;
        EmptyResponse response;
        grpc::ClientContext context;

        request.set_id(id);
        request.set_index(index);

        auto status =
            stub->GlEnableVertexAttribArray(&context, request, &response);
        if (!status.ok()) {
          LOG_WARN("GlEnableVertexAttribArray failed");
        }
      });

  remote_->job_queue()->Push(std::move(job));
}

void
RenderingUnit::GlDisableVertexAttribArray(uint32_t index)
{
  auto job =
      std::make_unique<Job>([id = id_, index, remote = remote_](bool cancel) {
        if (cancel) return;

        auto channel = remote->peer()->grpc_channel();

        auto stub = RenderingUnitService::NewStub(channel);

        GlDisableVertexAttribArrayRequest request;
        EmptyResponse response;
        grpc::ClientContext context;

        request.set_id(id);
        request.set_index(index);

        auto status =
            stub->GlDisableVertexAttribArray(&context, request, &response);
        if (!status.ok()) {
          LOG_WARN("GlDisableVertexAttribArray failed");
        }
      });

  remote_->job_queue()->Push(std::move(job));
}

void
RenderingUnit::GlVertexAttribPointer(uint32_t index, uint64_t buffer_id,
    int32_t size, uint64_t type, bool normalized, int32_t stride,
    uint64_t offset)
{
  auto job =
      std::make_unique<Job>([id = id_, index, buffer_id, size, type, normalized,
                                stride, offset, remote = remote_](bool cancel) {
        if (cancel) return;

        auto channel = remote->peer()->grpc_channel();

        auto stub = RenderingUnitService::NewStub(channel);

        GlVertexAttribPointerRequest request;
        EmptyResponse response;
        grpc::ClientContext context;

        request.set_id(id);
        request.set_index(index);
        request.set_buffer_id(buffer_id);
        request.set_size(size);
        request.set_type(type);
        request.set_normalized(normalized);
        request.set_stride(stride);
        request.set_offset(offset);

        auto status = stub->GlVertexAttribPointer(&context, request, &response);
        if (!status.ok()) {
          LOG_WARN("GlDisableVertexAttribArray failed");
        }
      });

  remote_->job_queue()->Push(std::move(job));
}

RenderingUnit::~RenderingUnit()
{
  auto job = std::make_unique<Job>([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = RenderingUnitService::NewStub(channel);

    DeleteResourceRequest request;
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
CreateRenderingUnit(std::shared_ptr<IRemote> remote, uint64_t virtual_object_id)
{
  auto rendering_unit = std::make_unique<RenderingUnit>(
      std::dynamic_pointer_cast<Remote>(remote));

  rendering_unit->Init(virtual_object_id);

  return rendering_unit;
}

}  // namespace zen::remote::server
