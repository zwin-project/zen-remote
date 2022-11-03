#include "server/peer.h"

namespace zen::remote::server {

uint64_t Peer::next_id_ = 0;

Peer::Peer(std::string host) : host_(host), id_(next_id_++) {}

std::string
Peer::host()
{
  return host_;
}

uint64_t
Peer::id()
{
  return id_;
}

}  // namespace zen::remote::server
