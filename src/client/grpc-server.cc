#include "client/grpc-server.h"

#include "client/service/async-session-service-caller.h"
#include "client/service/gl-base-technique.h"
#include "client/service/gl-buffer.h"
#include "client/service/gl-program.h"
#include "client/service/gl-shader.h"
#include "client/service/gl-texture.h"
#include "client/service/gl-vertex-array.h"
#include "client/service/rendering-unit.h"
#include "client/service/session.h"
#include "client/service/virtual-object.h"
#include "core/logger.h"

namespace zen::remote::client {

GrpcServer::GrpcServer(std::string host, uint16_t port, Remote *remote)
    : host_(host), port_(port), remote_(remote)
{
}

void
GrpcServer::Start()
{
  thread_ = std::thread([this] {
    grpc::ServerBuilder builder;

    std::string host_port = host_ + ":" + std::to_string(port_);

    builder.AddListeningPort(host_port, grpc::InsecureServerCredentials());

    std::vector<std::unique_ptr<service::IAsyncService>> services;

    services.emplace_back(new service::VirtualObjectServiceImpl(remote_));
    services.emplace_back(new service::RenderingUnitServiceImpl(remote_));
    services.emplace_back(new service::GlBufferServiceImpl(remote_));
    services.emplace_back(new service::GlShaderServiceImpl(remote_));
    services.emplace_back(new service::GlProgramServiceImpl(remote_));
    services.emplace_back(new service::GlTextureServiceImpl(remote_));
    services.emplace_back(new service::GlBaseTechniqueServiceImpl(remote_));
    services.emplace_back(new service::SessionServiceImpl(remote_));
    services.emplace_back(new service::GlVertexArrayServiceImpl(remote_));

    for (auto &service : services) {
      service->Register(builder);
    }

    completion_queue_ = builder.AddCompletionQueue();

    builder.SetMaxReceiveMessageSize(-1);

    server_ = builder.BuildAndStart();

    for (auto &service : services) {
      service->Listen(completion_queue_.get());
    }

    void *tag;
    bool ok = true;
    for (;;) {
      if (completion_queue_->Next(&tag, &ok) == false) break;

      auto caller = static_cast<service::IAsyncServiceCaller *>(tag);

      if (ok) {
        caller->Proceed();
      } else {
        caller->Cancel();
      }
    }
  });
}

GrpcServer::~GrpcServer()
{
  if (thread_.joinable() && server_) {
    server_->Shutdown(std::chrono::system_clock::now());
    completion_queue_->Shutdown();
    thread_.join();
  }
}

}  // namespace zen::remote::client
