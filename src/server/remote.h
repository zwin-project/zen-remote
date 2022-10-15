#pragma once

#include "core/connection/peer.h"
#include "core/context.h"
#include "job-queue.h"
#include "zen-remote/server/remote.h"

namespace zen::remote::server {

class Remote : public IRemote {
 public:
  enum SerialType {
    kObject = 0,
    kCount,
  };

  Remote(std::unique_ptr<ILoop> loop);

  void Start() override;

  void Stop() override;

  uint64_t NewSerial(SerialType type);

  std::unique_ptr<JobQueue>& job_queue();

  inline std::unique_ptr<connection::Peer>& peer();

 private:
  std::shared_ptr<Context> context_;
  std::unique_ptr<connection::Peer> peer_;

  uint64_t serials[SerialType::kCount] = {0};

  std::unique_ptr<JobQueue> job_queue_;
};

inline std::unique_ptr<JobQueue>&
Remote::job_queue()
{
  return job_queue_;
}

inline std::unique_ptr<connection::Peer>&
Remote::peer()
{
  return peer_;
}

}  // namespace zen::remote::server
