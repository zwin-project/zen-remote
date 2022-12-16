#pragma once

#include "core/common.h"
#include "server/job-queue.h"
#include "server/session-connection.h"
#include "zen-remote/loop.h"
#include "zen-remote/server/session.h"

namespace zen::remote::server {

class AsyncGrpcQueue;

/** Use this only in main thread */
class Session final : public ISession {
 public:
  enum SerialType {
    kResource = 0,
    kRequest = 1,
    kCount,
  };

  DISABLE_MOVE_AND_COPY(Session);
  Session(std::unique_ptr<ILoop> loop);
  ~Session();

  // TODO: make this async
  bool Connect(std::shared_ptr<IPeer> peer) override;

  int32_t GetPendingGrpcQueueCount() override;

  uint64_t NewSerial(SerialType type);

  inline JobQueue* job_queue();
  inline std::shared_ptr<AsyncGrpcQueue> grpc_queue();
  inline std::shared_ptr<SessionConnection> connection();
  inline uint64_t id();

 private:
  void HandleControlEvent(SessionConnection::ControlMessage message);

  void StartPingThread();
  void StopPingThread();

  JobQueue job_queue_;
  std::shared_ptr<AsyncGrpcQueue> grpc_queue_;     // shareable across threads
  std::shared_ptr<SessionConnection> connection_;  // sharable across threads
  std::unique_ptr<ILoop> loop_;

  bool should_ping_;
  std::thread ping_thread_;
  std::mutex ping_mutex_;
  std::condition_variable ping_cond_;

  uint64_t id_ = 0;
  bool connected_ = false;

  uint64_t serials_[SerialType::kCount] = {0};

  FdSource* control_event_source_;  // null before connected
  int pipe_[2];
};

inline JobQueue*
Session::job_queue()
{
  return &job_queue_;
}

inline std::shared_ptr<AsyncGrpcQueue>
Session::grpc_queue()
{
  return grpc_queue_;
}

inline std::shared_ptr<SessionConnection>
Session::connection()
{
  return connection_;
}

inline uint64_t
Session::id()
{
  return id_;
}

}  // namespace zen::remote::server
