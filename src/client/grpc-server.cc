#include "client/grpc-server.h"

#include "client/service/rendering-unit.h"
#include "core/logger.h"

namespace zen::remote::client {

void
GrpcServer::Start()
{
  thread_ = std::thread([this] {
    grpc::ServerBuilder builder;
    std::string host_port = host_ + ":" + std::to_string(port_);

    builder.AddListeningPort(host_port, grpc::InsecureServerCredentials());

    service::RenderingUnitServiceImpl rendering_unit_service(pool_);

    builder.RegisterService(&rendering_unit_service);

    server_ = builder.BuildAndStart();
    server_->Wait();
  });
}

GrpcServer::~GrpcServer()
{
  if (thread_.joinable() && server_) {
    server_->Shutdown();
    thread_.join();
  }
}

}  // namespace zen::remote::client
