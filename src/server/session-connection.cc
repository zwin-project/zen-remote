#include "server/session-connection.h"

#include "core/logger.h"

namespace zen::remote::server {

SessionConnection::SessionConnection(int control_fd, std::string host_port)
    : control_fd_(control_fd)
{
  grpc::ChannelArguments args;

  args.SetString(GRPC_ARG_OPTIMIZATION_TARGET, "latency");

  grpc_channel_ = grpc::CreateCustomChannel(
      host_port, grpc::InsecureChannelCredentials(), args);
}

void
SessionConnection::Disable()
{
  std::lock_guard<std::mutex> lock(mtx_);
  disabled_ = true;
  control_fd_ = -1;
}

void
SessionConnection::NotifyDisconnection()
{
  std::lock_guard<std::mutex> lock(mtx_);
  if (disabled_) return;
  uint8_t msg = kDisconnect;

  if (control_fd_ > 0) {
    write(control_fd_, &msg, sizeof(msg));
  }
}

}  // namespace zen::remote::server
