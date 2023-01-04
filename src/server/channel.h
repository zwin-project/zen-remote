#pragma once

#include "core/common.h"
#include "server/async-grpc-queue.h"
#include "server/job-queue.h"
#include "server/session.h"
#include "zen-remote/loop.h"
#include "zen-remote/server/channel.h"

namespace zen::remote::server {

class Channel final : public IChannel {
 public:
  enum SerialType {
    kRequest = 0,
    kResource,
    kCount,
  };

  DISABLE_MOVE_AND_COPY(Channel);
  Channel(int control_fd, std::string host_port, uint64_t session_id,
      std::shared_ptr<SessionSerial> session_serial);
  ~Channel();

  /**
   * This can be called from any thread.
   */
  void Disable();

  /**
   * Notify session the disconnection of this channel.
   * This can be called from any thread.
   */
  void NotifyDisconnection();

  void PushJob(std::unique_ptr<IJob> job);

  void PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase> caller);

  uint64_t NewSerial(SerialType type);

  uint32_t GetBusyness() override;

  inline std::shared_ptr<grpc::Channel> grpc_channel();
  inline bool enabled();
  inline uint64_t id();
  inline uint64_t session_id();

 private:
  JobQueue job_queue_;
  AsyncGrpcQueue grpc_queue_;
  std::shared_ptr<grpc::Channel> grpc_channel_;

  const uint64_t id_;
  const uint64_t session_id_;

  bool enabled_;
  int control_fd_ = -1;  // if enabled_ then control_fd_ is valid
  std::mutex mtx_;

  uint64_t serials_[SerialType::kCount] = {0};
  std::mutex serial_mtx_;

  std::shared_ptr<SessionSerial> session_serial_;
};

inline std::shared_ptr<grpc::Channel>
Channel::grpc_channel()
{
  return grpc_channel_;
}

inline bool
Channel::enabled()
{
  std::lock_guard<std::mutex> lock(mtx_);
  return enabled_;
}

inline uint64_t
Channel::id()
{
  return id_;
}

inline uint64_t
Channel::session_id()
{
  return session_id_;
}

}  // namespace zen::remote::server
