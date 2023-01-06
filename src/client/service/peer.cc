#include "client/service/peer.h"

#include "client/remote.h"
#include "client/service/async-service-caller.h"

namespace zen::remote::client::service {

PeerServiceImpl::PeerServiceImpl(Remote* remote) : remote_(remote) {}

void
PeerServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
PeerServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncServiceCaller<&PeerService::AsyncService::RequestProbe,
      &PeerServiceImpl::Probe>::Listen(&async_, this, completion_queue,
      remote_);
}

grpc::Status
PeerServiceImpl::Probe(grpc::ServerContext* /*context*/,
    const PeerProbeRequest* /*request*/, EmptyResponse* /*response*/)
{
  if (remote_->current()) {
    return grpc::Status(
        grpc::StatusCode::UNAVAILABLE, "Current Session Exists");
  } else {
    return grpc::Status::OK;
  }
}

}  // namespace zen::remote::client::service
