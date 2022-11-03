#include "server/gl-base-technique.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "gl-base-technique.grpc.pb.h"
#include "server/buffer.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/remote.h"
#include "server/serial-request-context.h"

namespace zen::remote::server {

GlBaseTechnique::GlBaseTechnique(std::shared_ptr<Remote> remote)
    : remote_(std::move(remote)),
      id_(remote_->NewSerial(Remote::SerialType::kResource))
{
}

void
GlBaseTechnique::Init(uint64_t rendering_unit_id)
{
  auto job =
      CreateJob([id = id_, rendering_unit_id, remote = remote_](bool cancel) {
        if (cancel) return;

        auto channel = remote->peer()->grpc_channel();

        auto stub = GlBaseTechniqueService::NewStub(channel);

        auto context = new SerialRequestContext(remote);
        auto request = new NewGlBaseTechniqueRequest();
        auto response = new EmptyResponse();

        request->set_id(id);
        request->set_rendering_unit_id(rendering_unit_id);

        stub->async()->New(context, request, response,
            [context, request, response](grpc::Status status) {
              if (!status.ok() && status.error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::New");
              }
              delete context;
              delete request;
              delete response;
            });
      });

  remote_->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  auto job =
      CreateJob([id = id_, remote = remote_, mode, first, count](bool cancel) {
        if (cancel) return;

        auto channel = remote->peer()->grpc_channel();

        auto stub = GlBaseTechniqueService::NewStub(channel);

        auto context = new SerialRequestContext(remote);
        auto request = new GlDrawArraysRequest();
        auto response = new EmptyResponse();

        request->set_id(id);
        request->set_mode(mode);
        request->set_first(first);
        request->set_count(count);

        stub->async()->GlDrawArrays(context, request, response,
            [context, request, response](grpc::Status status) {
              if (!status.ok() && status.error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::GlDrawArrays");
              }
              delete context;
              delete request;
              delete response;
            });
      });

  remote_->job_queue()->Push(std::move(job));
}

GlBaseTechnique::~GlBaseTechnique()
{
  auto job = CreateJob([id = id_, remote = remote_](bool cancel) {
    if (cancel) return;

    auto channel = remote->peer()->grpc_channel();

    auto stub = GlBaseTechniqueService::NewStub(channel);

    auto context = new SerialRequestContext(remote);
    auto request = new DeleteResourceRequest();
    auto response = new EmptyResponse();

    request->set_id(id);

    stub->async()->Delete(context, request, response,
        [context, request, response](grpc::Status status) {
          if (!status.ok() && status.error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBaseTechnique::Delete");
          }
          delete context;
          delete request;
          delete response;
        });
  });

  remote_->job_queue()->Push(std::move(job));
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

std::unique_ptr<IGlBaseTechnique>
CreateGlBaseTechnique(
    std::shared_ptr<IRemote> remote, uint64_t rendering_unit_id)
{
  auto gl_base_technique = std::make_unique<GlBaseTechnique>(
      std::dynamic_pointer_cast<Remote>(remote));

  gl_base_technique->Init(rendering_unit_id);

  return gl_base_technique;
}

}  // namespace zen::remote::server
