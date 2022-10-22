#pragma once

#include "core/common.h"
#include "zen-remote/server/remote.h"

namespace zen::remote {

class Context;

namespace connection {
class Peer;
}

}  // namespace zen::remote

namespace zen::remote::server {

class JobQueue;

class Remote : public IRemote {
 public:
  enum SerialType {
    kResource = 0,
    kCount,
  };

  DISABLE_MOVE_AND_COPY(Remote);
  Remote() = delete;
  Remote(std::unique_ptr<ILoop> loop);

  void Start() override;

  void Stop() override;

  uint64_t NewSerial(SerialType type);

  inline std::shared_ptr<Context> context();
  inline std::unique_ptr<connection::Peer>& peer();
  inline std::unique_ptr<JobQueue>& job_queue();

 private:
  std::shared_ptr<Context> context_;
  std::unique_ptr<connection::Peer> peer_;

  uint64_t serials[SerialType::kCount] = {0};

  std::unique_ptr<JobQueue> job_queue_;
};

inline std::shared_ptr<Context>
Remote::context()
{
  return context_;
}

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
