#pragma once

#include "core/common.h"

namespace zen::remote::server {

class SessionConnection {
 public:
  enum ControlMessage : uint8_t {
    kError = 0,
    kDisconnect,
  };

  DISABLE_MOVE_AND_COPY(SessionConnection);
  SessionConnection() = delete;
  SessionConnection(int control_fd, std::string host_port);

  /** Session calls this when session is about to be destroyed */
  void Disable();

  /** Program calls this to inform session to notify the gRPC connection lost */
  void NotifyDisconnection();

  inline std::shared_ptr<grpc::Channel> grpc_channel();

 private:
  std::shared_ptr<grpc::Channel> grpc_channel_;
  bool disabled_ = false;
  int control_fd_;
  std::mutex mtx_;
};

inline std::shared_ptr<grpc::Channel>
SessionConnection::grpc_channel()
{
  return grpc_channel_;
}

}  // namespace zen::remote::server
