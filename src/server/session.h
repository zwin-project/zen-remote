#pragma once

#include "core/common.h"
#include "server/job-queue.h"
#include "server/session-connection.h"
#include "server/session-serial.h"
#include "zen-remote/loop.h"
#include "zen-remote/server/session.h"

namespace zen::remote::server {

class AsyncGrpcQueue;
class Channel;

class Session final : public ISession,
                      public std::enable_shared_from_this<Session> {
 public:
  enum ControlMessage : uint8_t {
    kError = 0,
    kDisconnect,
  };

  DISABLE_MOVE_AND_COPY(Session);
  Session(std::unique_ptr<ILoop> loop);
  ~Session();

  // TODO: make this async
  bool Connect(std::shared_ptr<IPeer> peer) override;

  void AddChannel(std::weak_ptr<Channel> channel);

  inline uint64_t id();

  inline int control_fd();

  inline std::string host_port();

  inline std::shared_ptr<SessionSerial> serial();

 private:
  void HandleControlEvent(SessionConnection::ControlMessage message);

  void StartKeepalive();

  void DisableChannels();

  std::list<std::weak_ptr<Channel>> channels_;
  std::shared_ptr<Channel> session_channel_;
  std::unique_ptr<ILoop> loop_;

  uint64_t id_ = 0;
  bool connected_ = false;
  std::shared_ptr<SessionSerial> serial_;

  FdSource* control_event_source_ = nullptr;  // null before connected
  int pipe_[2];
  std::string host_port_;
};

inline uint64_t
Session::id()
{
  return id_;
}

inline int
Session::control_fd()
{
  return pipe_[1];
}

inline std::string
Session::host_port()
{
  return host_port_;
}

inline std::shared_ptr<SessionSerial>
Session::serial()
{
  return serial_;
}

}  // namespace zen::remote::server
