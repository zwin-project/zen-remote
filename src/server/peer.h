#include "core/common.h"
#include "zen-remote/server/peer.h"

namespace zen::remote::server {

class Peer final : public IPeer {
 public:
  DISABLE_MOVE_AND_COPY(Peer);
  Peer() = delete;
  Peer(std::string host);

  std::string host() override;
  uint64_t id() override;

 private:
  std::string host_;
  uint64_t id_;

  static uint64_t next_id_;
};

}  // namespace zen::remote::server
