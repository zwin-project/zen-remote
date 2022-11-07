#include "server/session.h"

#include "core/logger.h"
#include "server/serial-request-context.h"
#include "session.grpc.pb.h"

namespace zen::remote::server {

bool
Session::Connect(std::shared_ptr<IPeer> peer)
{
  auto host_port = peer->host() + ":" + std::to_string(kGrpcPort);
  grpc_channel_ =
      grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());

  auto stub = SessionService::NewStub(grpc_channel_);

  grpc::ClientContext context;
  NewSessionRequest request;
  NewSessionResponse response;

  auto status = stub->New(&context, request, &response);

  if (!status.ok()) {
    LOG_DEBUG("Failed to start session: %s", status.error_message().c_str());
    return false;
  }

  id_ = response.id();

  job_queue_.StartWorkerThread();

  return true;
}

uint64_t
Session::NewSerial(Session::SerialType type)
{
  return serials_[type]++;
}

std::unique_ptr<ISession>
CreateSession(std::unique_ptr<ILoop> /*loop*/)
{
  return std::make_unique<Session>();
}

}  // namespace zen::remote::server
