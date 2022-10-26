#include "server/rendering-unit.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "rendering-unit.grpc.pb.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/remote.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

RenderingUnit::RenderingUnit(std::shared_ptr<Remote> remote)
    : remote_(std::move(remote)),
      id_(remote_->NewSerial(Remote::SerialType::kResource))
{
}

void
RenderingUnit::Init(uint64_t virtual_object_id)
{
  auto job =
      CreateJob([id = id_, virtual_object_id, remote = remote_](bool cancel) {
        if (cancel) return;

        auto channel = remote->peer()->grpc_channel();

        auto stub = RenderingUnitService::NewStub(channel);

        auto context = new SerialRequestContext(remote);
        auto request = new NewRenderingUnitRequest();
        auto response = new EmptyResponse();

        request->set_id(id);
        request->set_virtual_object_id(virtual_object_id);

        stub->async()->New(context, request, response,
            [context, request, response](grpc::Status status) {
              if (!status.ok() && status.error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote RenderingUnit::New");
              }
              delete context;
              delete request;
              delete response;
            });
      });

  remote_->job_queue()->Push(std::move(job));
}

void
RenderingUnit::GlEnableVertexAttribArray(uint32_t index)
{
  auto job = CreateJob([id = id_, index, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = RenderingUnitService::NewStub(channel);

    auto context = new SerialRequestContext(remote);
    auto request = new GlEnableVertexAttribArrayRequest();
    auto response = new EmptyResponse();

    request->set_id(id);
    request->set_index(index);

    stub->async()->GlEnableVertexAttribArray(context, request, response,
        [context, request, response](grpc::Status status) {
          if (!status.ok() && status.error_code() != grpc::CANCELLED) {
            LOG_WARN(
                "Failed to call remote "
                "RenderingUnit::GlEnableVertexAttribArray");
          }
          delete context;
          delete request;
          delete response;
        });
  });

  remote_->job_queue()->Push(std::move(job));
}

void
RenderingUnit::GlDisableVertexAttribArray(uint32_t index)
{
  auto job = CreateJob([id = id_, index, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = RenderingUnitService::NewStub(channel);

    auto context = new SerialRequestContext(remote);
    auto request = new GlDisableVertexAttribArrayRequest();
    auto response = new EmptyResponse();

    request->set_id(id);
    request->set_index(index);

    stub->async()->GlDisableVertexAttribArray(context, request, response,
        [context, request, response](grpc::Status status) {
          if (!status.ok() && status.error_code() != grpc::CANCELLED) {
            LOG_WARN(
                "Failed to call remote "
                "RenderingUnit::GlDisableVertexAttribArray");
          }
          delete context;
          delete request;
          delete response;
        });
  });

  remote_->job_queue()->Push(std::move(job));
}

void
RenderingUnit::GlVertexAttribPointer(uint32_t index, uint64_t buffer_id,
    int32_t size, uint64_t type, bool normalized, int32_t stride,
    uint64_t offset)
{
  auto job = CreateJob([id = id_, index, buffer_id, size, type, normalized,
                           stride, offset, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = RenderingUnitService::NewStub(channel);

    auto context = new SerialRequestContext(remote);
    auto request = new GlVertexAttribPointerRequest();
    auto response = new EmptyResponse();

    request->set_id(id);
    request->set_index(index);
    request->set_buffer_id(buffer_id);
    request->set_size(size);
    request->set_type(type);
    request->set_normalized(normalized);
    request->set_stride(stride);
    request->set_offset(offset);

    stub->async()->GlVertexAttribPointer(context, request, response,
        [context, request, response](grpc::Status status) {
          if (!status.ok() && status.error_code() != grpc::CANCELLED) {
            LOG_WARN(
                "Failed to call remote "
                "RenderingUnit::GlDisableVertexAttribArray");
          }
          delete context;
          delete request;
          delete response;
        });
  });

  remote_->job_queue()->Push(std::move(job));
}

RenderingUnit::~RenderingUnit()
{
  auto job = CreateJob([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = RenderingUnitService::NewStub(channel);

    auto context = new SerialRequestContext(remote);
    auto request = new DeleteResourceRequest();
    auto response = new EmptyResponse();

    request->set_id(id);

    stub->async()->Delete(context, request, response,
        [context, request, response](grpc::Status status) {
          if (!status.ok() && status.error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote RenderingUnit::Delete");
          }
          delete context;
          delete request;
          delete response;
        });
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
