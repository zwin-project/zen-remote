#include "server/gl-buffer.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "gl-buffer.grpc.pb.h"
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
  uint64_t id = id_;
  auto remote = remote_;

  auto job = std::make_unique<Job>([id, remote](bool cancel) {
    if (cancel) return;

    auto &peer = remote->peer();

    auto host_port = peer->endpoint().address().to_string() + ":" +
                     std::to_string(kGrpcPort);

    auto channel =
        grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());

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

GlBuffer::~GlBuffer()
{
  uint64_t id = id_;
  auto remote = remote_;

  auto job = std::make_unique<Job>([id, remote](bool cancel) {
    if (cancel) return;

    auto &peer = remote->peer();

    auto host_port = peer->endpoint().address().to_string() + ":" +
                     std::to_string(kGrpcPort);

    auto channel =
        grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());

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
