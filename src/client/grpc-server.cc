#include "client/grpc-server.h"

#include "client/service/gl-buffer.h"
#include "client/service/rendering-unit.h"
#include "client/service/serial-async-caller.h"
#include "client/service/virtual-object.h"
#include "core/logger.h"

namespace zen::remote::client {

GrpcServer::GrpcServer(std::string host, uint16_t port, ResourcePool *pool)
    : host_(host), port_(port), pool_(pool)
{
}

void
GrpcServer::Start()
{
  thread_ = std::thread([this] {
    grpc::ServerBuilder builder;

    std::string host_port = host_ + ":" + std::to_string(port_);

    builder.AddListeningPort(host_port, grpc::InsecureServerCredentials());

    std::vector<std::unique_ptr<service::ISerialAsyncService>> services;
    SerialCommandQueue command_queue;

    services.emplace_back(new service::VirtualObjectServiceImpl(pool_));
    services.emplace_back(new service::RenderingUnitServiceImpl(pool_));
    services.emplace_back(new service::GlBufferServiceImpl(pool_));

    for (auto &service : services) {
      service->Register(builder);
    }

    completion_queue_ = builder.AddCompletionQueue();

    builder.SetMaxReceiveMessageSize(-1);

    server_ = builder.BuildAndStart();

    for (auto &service : services) {
      service->Listen(completion_queue_.get(), &command_queue);
    }

    void *tag;
    bool ok = true;
    for (;;) {
      if (completion_queue_->Next(&tag, &ok) == false) break;
      if (!ok) {
        LOG_ERROR("Failed to poll gRPC queue");
        break;
      }
      static_cast<service::ISerialAsyncCaller *>(tag)->Proceed();
    }
    // FIXME: Some SerialAsyncCallers should not be deleted
  });
}

GrpcServer::~GrpcServer()
{
  if (thread_.joinable() && server_) {
    server_->Shutdown();
    completion_queue_->Shutdown();
    thread_.join();
  }
}

}  // namespace zen::remote::client
