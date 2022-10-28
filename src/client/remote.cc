#include "client/remote.h"

#include "client/grpc-server.h"
#include "core/connection/peer.h"
#include "core/logger.h"

namespace zen::remote::client {

Remote::Remote(std::unique_ptr<ILoop> loop)
    : context_(std::make_unique<Context>(std::move(loop)))
{
}

void
Remote::Start()
{
  peer_ = std::make_unique<connection::Peer>(
      connection::Peer::Target::kServer, context_);
  peer_->signals.discoverd.connect([this] {
    std::string server_ip = peer_->endpoint().address().to_string();
    LOG_INFO("Server Found: %s", server_ip.c_str());
  });
  peer_->StartDiscover();

  grpc_server_ =
      std::make_unique<GrpcServer>("0.0.0.0", kGrpcPort, &this->pool_);
  grpc_server_->Start();
}

void
Remote::Stop()
{
  // TODO:
}

void
Remote::UpdateScene()
{
  pool_.UpdateRenderingState();
}

void
Remote::Render(Camera *camera)
{
  pool_.virtual_objects()->ForEach(
      [camera](const std::shared_ptr<VirtualObject> &virtual_object) {
        if (virtual_object->commited()) virtual_object->Render(camera);
      });
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::remote::client
