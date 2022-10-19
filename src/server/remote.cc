#include "server/remote.h"

#include "core/connection/peer.h"
#include "core/logger.h"
#include "job-queue.h"
#include "zen-remote/server/remote.h"

namespace zen::remote::server {
namespace ip = boost::asio::ip;

Remote::Remote(std::unique_ptr<ILoop> loop)
    : context_(std::make_unique<Context>(std::move(loop))),
      job_queue_(std::make_unique<JobQueue>())
{
  peer_ = std::make_unique<connection::Peer>(
      connection::Peer::Target::kClient, context_);

  peer_->signals.discoverd.connect([this] {
    std::string client_ip = peer_->endpoint().address().to_string();
    LOG_INFO("Client Found: %s", client_ip.c_str());

    job_queue_->StartWorkerThread();
  });
}

void
Remote::Start()
{
  peer_->StartDiscover();
}

void
Remote::Stop()
{
  job_queue_ = std::make_unique<JobQueue>();
}

uint64_t
Remote::NewSerial(Remote::SerialType type)
{
  return serials[type]++;
}

std::unique_ptr<IRemote>
CreateRemote(std::unique_ptr<ILoop> loop)
{
  return std::make_unique<Remote>(std::move(loop));
}

}  // namespace zen::remote::server
