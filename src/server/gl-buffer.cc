#include "server/gl-buffer.h"

#include "core/logger.h"
#include "gl-buffer.grpc.pb.h"
#include "server/buffer.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

GlBuffer::GlBuffer(std::shared_ptr<Session> session)
    : session_(std::move(session)), id_(session_->NewSerial(Session::kResource))
{
}

void
GlBuffer::Init()
{
  auto job = CreateJob([id = id_, session = session_](bool cancel) {
    if (cancel) return;

    auto channel = session->grpc_channel();

    auto stub = GlBufferService::NewStub(channel);

    auto context = new SerialRequestContext(session.get());
    auto request = new NewResourceRequest();
    auto response = new EmptyResponse();

    request->set_id(id);

    stub->async()->New(context, request, response,
        [context, request, response](grpc::Status status) {
          if (!status.ok() && status.error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBuffer::New");
          }
          delete context;
          delete request;
          delete response;
        });
  });

  session_->job_queue()->Push(std::move(job));
}

void
GlBuffer::GlBufferData(std::unique_ptr<IBuffer> buffer, uint64_t target,
    size_t size, uint64_t usage)
{
  auto job =
      CreateJob([id = id_, session = session_, buffer = std::move(buffer),
                    target, size, usage](bool cancel) {
        if (cancel) return;

        auto channel = session->grpc_channel();

        auto stub = GlBufferService::NewStub(channel);

        auto context = new SerialRequestContext(session.get());
        auto request = new GlBufferDataRequest();
        auto response = new EmptyResponse();

        request->set_id(id);
        request->set_target(target);
        request->set_usage(usage);
        request->set_data(buffer->data(), size);

        stub->async()->GlBufferData(context, request, response,
            [context, request, response](grpc::Status status) {
              if (!status.ok() && status.error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBuffer::GlBufferData");
              }
              delete context;
              delete request;
              delete response;
            });
      });

  session_->job_queue()->Push(std::move(job));
}

GlBuffer::~GlBuffer()
{
  auto job = CreateJob([id = id_, session = session_](bool cancel) {
    if (cancel) return;

    auto channel = session->grpc_channel();

    auto stub = GlBufferService::NewStub(channel);

    auto context = new SerialRequestContext(session.get());
    auto request = new DeleteResourceRequest();
    auto response = new EmptyResponse();

    request->set_id(id);

    stub->async()->Delete(context, request, response,
        [context, request, response](grpc::Status status) {
          if (!status.ok() && status.error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBuffer::Delete");
          }
          delete context;
          delete request;
          delete response;
        });
  });

  session_->job_queue()->Push(std::move(job));
}

uint64_t
GlBuffer::id()
{
  return id_;
}

std::unique_ptr<IGlBuffer>
CreateGlBuffer(std::shared_ptr<ISession> session)
{
  auto gl_buffer =
      std::make_unique<GlBuffer>(std::dynamic_pointer_cast<Session>(session));

  gl_buffer->Init();

  return gl_buffer;
}

}  // namespace zen::remote::server
