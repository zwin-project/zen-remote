#include "server/peer-manager.h"

#include "core/connection/packet.h"
#include "core/logger.h"
#include "server/peer.h"

namespace zen::remote::server {

namespace ip = boost::asio::ip;
namespace asio = boost::asio;

bool
PeerManager::Init()
{
  return ListenUdpBroadcast();
}

std::shared_ptr<IPeer>
PeerManager::Get(uint64_t peer_id)
{
  auto result = std::find_if(peers_.begin(), peers_.end(),
      [peer_id](std::shared_ptr<Peer>& peer) { return peer->id() == peer_id; });

  if (result == peers_.end()) {
    return std::shared_ptr<IPeer>();
  } else {
    return *result;
  }
}

bool
PeerManager::ListenUdpBroadcast()
{
  boost::system::error_code err;
  udp_socket_ = ip::udp::socket(io_context_);
  if (udp_socket_source_) loop_->RemoveFd(udp_socket_source_.get());

  udp_socket_.open(ip::udp::v4(), err);
  if (err) {
    LOG_ERROR("Failed to open UDP socket: %s", err.what().c_str());
    return false;
  }

  udp_socket_.set_option(ip::udp::socket::reuse_address(true));
  udp_socket_.set_option(asio::socket_base::broadcast(true));

  udp_socket_.bind(
      ip::udp::endpoint(ip::address_v4::any(), kDiscoverPort), err);
  if (err) {
    LOG_ERROR("Failed to bind UDP socket to port %d: %s", kDiscoverPort,
        err.what().c_str());
    udp_socket_.close();
    return false;
  }

  udp_socket_source_ = std::make_unique<FdSource>();
  udp_socket_source_->fd = udp_socket_.native_handle();
  udp_socket_source_->mask = FdSource::kReadable;
  udp_socket_source_->callback = std::bind(&PeerManager::AcceptUdpBroadcast,
      this, std::placeholders::_1, std::placeholders::_2);

  loop_->AddFd(udp_socket_source_.get());

  return true;
}

void
PeerManager::AcceptUdpBroadcast(int /*fd*/, uint32_t mask)
{
  boost::system::error_code err;
  ip::udp::endpoint client_endpoint;
  connection::DiscoverPacket packet;

  if (mask != FdSource::kReadable) {
    on_error("Error on socket listening for UDP broadcast");
    // TODO: recovery
    return;
  }

  size_t len = udp_socket_.receive_from(
      boost::asio::buffer(&packet, sizeof(packet)), client_endpoint, 0, err);
  if (err) {
    on_error(Format("Failed to read from UDP socket: %s", err.what().c_str()));
    return;
  }

  if (len != sizeof(packet)) {
    LOG_DEBUG("Unexpected broadcast length");
    return;
  }

  if (std::memcmp(DISCOVER_PACKET_PREAMBLE, packet.preamble,
          sizeof(packet.preamble)) != 0) {
    LOG_DEBUG("Unexpected broadcast packet received");
    return;
  }

  if (packet.version != DISCOVER_PACKET_VERSION) {
    on_warn("Client version mismatch");
    return;
  }

  for (auto& peer : peers_) {
    if (peer->host() == client_endpoint.address().to_string()) {
      peer->Ping();
      return;
    }
  }

  auto peer = CreatePeer(client_endpoint.address().to_string(), loop_);
  if (!peer) {
    LOG_ERROR("Failed to create a peer");
    return;
  };

  peer->on_expired = [this, id = peer->id()]() {
    peers_.remove_if(
        [id](std::shared_ptr<Peer>& peer) { return peer->id() == id; });
    on_peer_lost(id);
  };

  peer->Ping();

  peers_.push_back(peer);

  on_peer_discover(peer->id());
}

PeerManager::PeerManager(std::unique_ptr<ILoop> loop)
    : loop_(std::move(loop)), udp_socket_(io_context_)
{
}

PeerManager::~PeerManager()
{
  if (udp_socket_source_) loop_->RemoveFd(udp_socket_source_.get());
}

std::unique_ptr<IPeerManager>
CreatePeerManager(std::unique_ptr<ILoop> loop)
{
  auto manager = std::make_unique<PeerManager>(std::move(loop));

  if (manager->Init() == false) {
    return std::unique_ptr<IPeerManager>();
  }

  return manager;
}

}  // namespace zen::remote::server
