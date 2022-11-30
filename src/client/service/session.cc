#include "client/service/session.h"

#include "client/remote.h"
#include "client/service/async-service-caller.h"
#include "client/service/async-session-ping-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

SessionServiceImpl::SessionServiceImpl(Remote* remote) : remote_(remote) {}

void
SessionServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
SessionServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncServiceCaller<&SessionService::AsyncService::RequestNew,
      &SessionServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionPingCaller::Listen(&async_, completion_queue, remote_);
}

grpc::Status
SessionServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewSessionRequest* /*request*/, NewSessionResponse* response)
{
  auto id = remote_->session_manager()->ResetCurrent();

  response->set_id(id);

  if (id == 0)
    return grpc::Status(grpc::ABORTED, "Failed to create a session");
  else
    return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
