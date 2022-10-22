#include "client/grpc-server.h"

#include "client/service/gl-buffer.h"
#include "client/service/rendering-unit.h"
#include "client/service/virtual-object.h"

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

    service::GlBufferServiceImpl gl_buffer_service(pool_);
    service::RenderingUnitServiceImpl rendering_unit_service(pool_);
    service::VirtualObjectServiceImpl virtual_object_service(pool_);

    builder.RegisterService(&gl_buffer_service);
    builder.RegisterService(&rendering_unit_service);
    builder.RegisterService(&virtual_object_service);

    builder.SetMaxReceiveMessageSize(-1);

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
