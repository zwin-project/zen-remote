#pragma once

#include "core/common.h"
#include "server/job-queue.h"
#include "zen-remote/loop.h"
#include "zen-remote/server/session.h"

namespace zen::remote::server {

class AsyncGrpcQueue;
class SessionConnection;

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

  uint64_t NewSerial(SerialType type);

  inline JobQueue* job_queue();
  inline std::shared_ptr<AsyncGrpcQueue> grpc_queue();
  inline std::shared_ptr<SessionConnection> connection();
  inline uint64_t id();

 private:
  JobQueue job_queue_;
  std::shared_ptr<AsyncGrpcQueue> grpc_queue_;     // shareable across threads
  std::shared_ptr<SessionConnection> connection_;  // sharable across threads
  uint64_t serials_[SerialType::kCount] = {0};
  uint64_t id_ = 0;
  std::unique_ptr<ILoop> loop_;
  FdSource* control_event_source_;  // null before connected
  bool connected_ = false;

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
