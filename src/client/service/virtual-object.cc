#include "client/service/virtual-object.h"

#include "client/remote.h"
#include "client/resource-pool.h"
#include "client/service/async-session-service-caller.h"
#include "client/session.h"

namespace zen::remote::client::service {

VirtualObjectServiceImpl::VirtualObjectServiceImpl(Remote* remote)
    : remote_(remote)
{
}

void
VirtualObjectServiceImpl::Register(grpc::ServerBuilder& builder)
{
  builder.RegisterService(&async_);
}

void
VirtualObjectServiceImpl::Listen(grpc::ServerCompletionQueue* completion_queue)
{
  AsyncSessionServiceCaller<&VirtualObjectService::AsyncService::RequestNew,
      &VirtualObjectServiceImpl::New>::Listen(&async_, this, completion_queue,
      remote_);

  AsyncSessionServiceCaller<&VirtualObjectService::AsyncService::RequestDelete,
      &VirtualObjectServiceImpl::Delete>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<&VirtualObjectService::AsyncService::RequestCommit,
      &VirtualObjectServiceImpl::Commit>::Listen(&async_, this,
      completion_queue, remote_);

  AsyncSessionServiceCaller<&VirtualObjectService::AsyncService::RequestMove,
      &VirtualObjectServiceImpl::Move>::Listen(&async_, this, completion_queue,
      remote_);
}

grpc::Status
VirtualObjectServiceImpl::New(grpc::ServerContext* /*context*/,
    const NewResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto virtual_object = std::make_unique<VirtualObject>(
      request->id(), remote_->update_rendering_queue());

  pool->virtual_objects()->Add(std::move(virtual_object));

  return grpc::Status::OK;
}

grpc::Status
VirtualObjectServiceImpl::Delete(grpc::ServerContext* /*context*/,
    const DeleteResourceRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  pool->virtual_objects()->ScheduleRemove(request->id());

  return grpc::Status::OK;
}

grpc::Status
VirtualObjectServiceImpl::Commit(grpc::ServerContext* /*context*/,
    const VirtualObjectCommitRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto virtual_object = pool->virtual_objects()->Get(request->id());

  virtual_object->Commit();

  return grpc::Status::OK;
}

grpc::Status
VirtualObjectServiceImpl::Move(grpc::ServerContext* /*context*/,
    const VirtualObjectMoveRequest* request, EmptyResponse* /*response*/)
{
  auto pool = remote_->session_manager()->current()->pool();

  auto virtual_object = pool->virtual_objects()->Get(request->id());

  glm::vec3 position;
  glm::quat quaternion;

  std::memcpy(&position, request->position().data(), sizeof(glm::vec3));
  std::memcpy(&quaternion, request->quaternion().data(), sizeof(glm::quat));

  virtual_object->Move(position, quaternion);

  return grpc::Status::OK;
}

}  // namespace zen::remote::client::service
