#pragma once

#include "core/common.h"
#include "server/job-queue.h"
#include "zen-remote/server/session.h"

namespace zen::remote::server {

class Session final : public ISession {
 public:
  enum SerialType {
    kResource = 0,
    kRequest = 1,
    kCount,
  };

  DISABLE_MOVE_AND_COPY(Session);
  Session() = default;

  // TODO: make this async
  bool Connect(std::shared_ptr<IPeer> peer) override;

  uint64_t NewSerial(SerialType type);

  inline JobQueue* job_queue();
  inline std::shared_ptr<grpc::Channel> grpc_channel();
  inline uint64_t id();

 private:
  JobQueue job_queue_;
  std::shared_ptr<grpc::Channel> grpc_channel_;
  uint64_t serials_[SerialType::kCount] = {0};
  uint64_t id_ = 0;
};

inline JobQueue*
Session::job_queue()
{
  return &job_queue_;
}

inline std::shared_ptr<grpc::Channel>
Session::grpc_channel()
{
  return grpc_channel_;
}

inline uint64_t
Session::id()
{
  return id_;
}

}  // namespace zen::remote::server
