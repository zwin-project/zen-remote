#include "server/gl-buffer.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "gl-buffer.grpc.pb.h"
#include "server/buffer.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/remote.h"

namespace zen::remote::server {

GlBuffer::GlBuffer(std::shared_ptr<Remote> remote)
    : remote_(std::move(remote)),
      id_(remote_->NewSerial(Remote::SerialType::kResource))
{
}

void
GlBuffer::Init()
{
  auto job = CreateJob([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = GlBufferService::NewStub(channel);

    NewResourceRequest request;
    EmptyResponse response;
    grpc::ClientContext context;

    request.set_id(id);

    auto status = stub->New(&context, request, &response);
    if (!status.ok()) {
      LOG_WARN("Failed to create a new GL buffer");
    }
  });

  remote_->job_queue()->Push(std::move(job));
}

void
GlBuffer::GlBufferData(
    std::unique_ptr<IBuffer> buffer, size_t size, uint64_t usage)
{
  auto job = CreateJob([id = id_, remote = remote_, buffer = std::move(buffer),
                           size, usage](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = GlBufferService::NewStub(channel);

    GlBufferDataRequest request;
    EmptyResponse response;
    grpc::ClientContext context;

    request.set_id(id);
    request.set_usage(usage);
    request.set_data(buffer->data(), size);

    auto status = stub->GlBufferData(&context, request, &response);
    if (!status.ok()) {
      LOG_WARN("Failed to call remote GlBufferData");
    }
  });

  remote_->job_queue()->Push(std::move(job));
}

GlBuffer::~GlBuffer()
{
  auto job = CreateJob([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = GlBufferService::NewStub(channel);

    DeleteResourceRequest request;
    EmptyResponse response;
    grpc::ClientContext context;

    request.set_id(id);

    auto status = stub->Delete(&context, request, &response);
    if (!status.ok()) {
      LOG_WARN("Failed to destroy a gl buffer");
    }
  });

  remote_->job_queue()->Push(std::move(job));
}

uint64_t
GlBuffer::id()
{
  return id_;
}

std::unique_ptr<IGlBuffer>
CreateGlBuffer(std::shared_ptr<IRemote> remote)
{
  auto gl_buffer =
      std::make_unique<GlBuffer>(std::dynamic_pointer_cast<Remote>(remote));

  gl_buffer->Init();

  return gl_buffer;
}

}  // namespace zen::remote::server
