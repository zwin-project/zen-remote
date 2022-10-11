#include "client/remote.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "core/connection/peer.h"
#include "core/logger.h"
#include "ping.grpc.pb.h"
#include "ping.pb.h"
#include "zen-remote/remote.h"

namespace zen::remote {

// FIXME: This is a temporary code to check if gRPC works.
class GlobalServiceImpl final : public Global::Service {
  virtual ::grpc::Status Ping(::grpc::ServerContext* /*context*/,
      const ::zen::remote::PingReuqest* /*request*/,
      ::zen::remote::PingResponse* /*response*/) override
  {
    LOG_DEBUG("Pong!!");
    return ::grpc::Status::OK;
  }
};

void
Remote::Start()
{
  peer_ = std::make_unique<connection::Peer>(
      connection::Peer::Target::kServer, context_);
  peer_->signals.discoverd.connect([this] {
    LOG_INFO(
        "Server Found: %s", peer_->endpoint().address().to_string().c_str());
  });
  peer_->StartDiscover();

  {  // FIXME: This is a temporary code to check if gRPC works.
    std::thread([] {
      std::string server_address("0.0.0.0:50051");
      GlobalServiceImpl service;

      grpc::EnableDefaultHealthCheckService(true);
      grpc::ServerBuilder builder;

      builder.AddListeningPort(
          server_address, grpc::InsecureServerCredentials());
      builder.RegisterService(&service);
      std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
      LOG_DEBUG("gRPC server listening on %s", server_address.c_str());

      server->Wait();
    }).detach();
  }
}

void
Remote::Stop()
{
  // TODO:
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::remote
