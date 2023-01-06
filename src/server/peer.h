#include "core/common.h"
#include "zen-remote/loop.h"
#include "zen-remote/server/peer.h"

namespace zen::remote::server {

constexpr uint32_t kPeerPingTimeoutMsec = 4500;

class Peer final : public IPeer {
 public:
  DISABLE_MOVE_AND_COPY(Peer);
  Peer() = delete;
  Peer(std::string host, std::shared_ptr<ILoop> loop, bool wired);
  ~Peer();

  bool Init();

  void Ping();

  std::string host() override;
  bool wired() override;
  uint64_t id() override;

  std::function<void()> on_expired;

 private:
  void Expire();

  const std::string host_;
  const bool wired_;
  uint64_t id_;
  std::shared_ptr<ILoop> loop_;
  int ping_timer_fd_ = -1;
  std::unique_ptr<FdSource> ping_timer_source_;
  bool alive_ = true;

  static uint64_t next_id_;
};

std::shared_ptr<Peer> CreatePeer(
    std::string host, std::shared_ptr<ILoop> loop, bool wired);

}  // namespace zen::remote::server
