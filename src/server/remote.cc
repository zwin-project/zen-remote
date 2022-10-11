#include "server/remote.h"

#include <grpcpp/grpcpp.h>

#include "core/connection/peer.h"
#include "core/logger.h"
#include "ping.grpc.pb.h"
#include "ping.pb.h"
#include "zen-remote/remote.h"

namespace zen::remote {
namespace ip = boost::asio::ip;

void
Remote::Start()
{
  peer_ = std::make_unique<connection::Peer>(
      connection::Peer::Target::kClient, context_);
  peer_->signals.discoverd.connect([this] {
    std::string client_ip = peer_->endpoint().address().to_string();

    LOG_INFO("Client Found: %s", client_ip.c_str());

    {  // FIXME: This is a temporary code to check if gRPC works.
      auto channel = grpc::CreateChannel(
          client_ip + ":50051", grpc::InsecureChannelCredentials());
      auto stub = Global::NewStub(channel);

      PingReuqest request;

      PingResponse response;

      grpc::ClientContext context;

      LOG_DEBUG("Ping ...");
      grpc::Status status = stub->Ping(&context, request, &response);

      if (status.ok()) {
        LOG_DEBUG("Ping ... Ok");
      }
    }
  });
  peer_->StartDiscover();
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
