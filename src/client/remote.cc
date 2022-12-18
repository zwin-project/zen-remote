#include "client/remote.h"

#include "client/grpc-server.h"
#include "client/resource-pool.h"
#include "client/session.h"
#include "core/logger.h"

namespace zen::remote::client {

Remote::Remote(std::unique_ptr<ILoop> loop) : loop_(std::move(loop)) {}

Remote::~Remote()
{
  std::lock_guard<std::mutex> lock(current_session_mutex_);
  if (current_session_) {
    current_session_->Shutdown();
    current_session_.reset();
  }
  discovery_broadcast_.StopAndWait();
}

void
Remote::StartGrpcServer()
{
  grpc_server_ = std::make_unique<GrpcServer>("0.0.0.0", kGrpcPort, this);
  grpc_server_->Start();
}

void
Remote::EnableSession()
{
  std::lock_guard<std::mutex> lock(current_session_mutex_);
  session_is_enabled_ = true;
  if (!current_session_) discovery_broadcast_.StartIfNotRunning();
}

void
Remote::DisableSession()
{
  std::lock_guard<std::mutex> lock(current_session_mutex_);
  session_is_enabled_ = false;
  discovery_broadcast_.StopAndWait();
  if (current_session_) {
    current_session_->Shutdown();
    current_session_.reset();
  }
}

void
Remote::UpdateScene()
{
  auto execute_count = update_rendering_queue_.commit_count();

  for (uint i = 0; i < execute_count; i++) {
    if (update_rendering_queue_.ExecuteOnce() == false) break;
  }
}

std::shared_ptr<Session>
Remote::ResetSession()
{
  std::lock_guard<std::mutex> lock(current_session_mutex_);

  if (!session_is_enabled_) return std::shared_ptr<Session>();

  current_session_ = std::make_shared<Session>();

  discovery_broadcast_.StopAndWait();

  return current_session_;
}

void
Remote::ClearSession()
{
  std::lock_guard<std::mutex> lock(current_session_mutex_);
  if (current_session_) {
    current_session_->Shutdown();
    current_session_.reset();
  }
  if (session_is_enabled_) discovery_broadcast_.StartIfNotRunning();
}

void
Remote::Render(Camera* camera)
{
  auto session = current_session_;
  if (!session) return;

  auto pool = session->pool();
  if (!pool) return;

  glEnable(GL_DEPTH_TEST);

  pool->virtual_objects()->ForEach(
      [camera](const std::shared_ptr<VirtualObject>& virtual_object) {
        if (virtual_object->commited()) virtual_object->Render(camera);
      });
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::remote::client
