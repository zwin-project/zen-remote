#include "client/service/session.h"

#include "client/remote.h"
#include "client/service/async-service-caller.h"
#include "client/service/async-session-keepalive-caller.h"
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

  AsyncServiceCaller<&SessionService::AsyncService::RequestShutdown,
      &SessionServiceImpl::Shutdown>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionKeepaliveCaller::Listen(&async_, completion_queue, remote_);
}

grpc::Status
SessionServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewSessionRequest* /*request*/, NewSessionResponse* response)
{
  auto session = remote_->ResetSession();

  if (!session) {
    return grpc::Status(grpc::ABORTED, "Failed to create a session");
  } else {
    response->set_id(session->id());
    return grpc::Status::OK;
  }
}

grpc::Status
SessionServiceImpl::Shutdown(grpc::ServerContext* /*context*/,
    const SessionShutdownRequest* request, EmptyResponse* /*response*/)
{
  remote_->ClearSession(request->id());
  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
