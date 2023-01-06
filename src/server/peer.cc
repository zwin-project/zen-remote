#include "server/peer.h"

namespace zen::remote::server {

uint64_t Peer::next_id_ = 0;

Peer::Peer(std::string host, std::shared_ptr<ILoop> loop, bool wired)
    : host_(host), wired_(wired), id_(next_id_++), loop_(std::move(loop))
{
}

Peer::~Peer()
{
  if (ping_timer_fd_ >= 0) loop_->RemoveFd(ping_timer_source_.get());
}

bool
Peer::Init()
{
  ping_timer_fd_ = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  if (ping_timer_fd_ < 0) return false;

  ping_timer_source_ = std::make_unique<FdSource>();
  ping_timer_source_->fd = ping_timer_fd_;
  ping_timer_source_->mask = FdSource::kReadable;
  ping_timer_source_->callback = [this](int /*fd*/, uint32_t /*mask*/) {
    Expire();
  };

  loop_->AddFd(ping_timer_source_.get());

  return true;
}

void
Peer::Ping()
{
  if (!alive_) return;
  struct timespec deadline;
  struct itimerspec timer;

  clock_gettime(CLOCK_MONOTONIC, &deadline);

  deadline.tv_nsec += (kPeerPingTimeoutMsec % 1000) * 1000000L;
  deadline.tv_sec += kPeerPingTimeoutMsec / 1000;
  if (deadline.tv_nsec >= 1000000000L) {
    deadline.tv_nsec -= 1000000000L;
    deadline.tv_sec += 1;
  }

  timer.it_interval.tv_nsec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_value = deadline;

  if (timerfd_settime(ping_timer_fd_, TFD_TIMER_ABSTIME, &timer, NULL) != 0) {
    Expire();
  }
}

void
Peer::Expire()
{
  if (alive_ == false) return;
  alive_ = false;
  on_expired();
}

std::string
Peer::host()
{
  return host_;
}

bool
Peer::wired()
{
  return wired_;
}

uint64_t
Peer::id()
{
  return id_;
}

std::shared_ptr<Peer>
CreatePeer(std::string host, std::shared_ptr<ILoop> loop, bool wired)
{
  auto peer = std::make_shared<Peer>(host, loop, wired);
  if (peer->Init() == false) return std::shared_ptr<Peer>();
  return peer;
}

}  // namespace zen::remote::server
