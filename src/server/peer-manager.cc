#include "server/peer-manager.h"

#include "core/connection/packet.h"
#include "core/logger.h"
#include "peer.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/peer.h"

namespace zen::remote::server {

namespace ip = boost::asio::ip;
namespace asio = boost::asio;

constexpr char kPortForwardedPeerHost[] = "127.0.0.1";

bool
PeerManager::Init()
{
  if (!notifier_.Init()) {
    return false;
  }

  StartPortForwardedPeerProber();
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

  auto peer = CreatePeer(client_endpoint.address().to_string(), loop_, false);
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

void
PeerManager::StartPortForwardedPeerProber()
{
  prober_.enable = true;
  port_forwarded_peer_prober_ = std::thread([this] {
    std::string host = kPortForwardedPeerHost;
    auto host_port = host + ":" + std::to_string(kGrpcPort);

    AsyncGrpcQueue grpc_queue;
    grpc_queue.Start();

    for (;;) {
      std::unique_lock<std::mutex> lock(prober_.mtx);

      grpc::ChannelArguments args;
      auto channel = grpc::CreateCustomChannel(
          host_port, grpc::InsecureChannelCredentials(), args);

      auto stub = PeerService::NewStub(channel);
      auto context = std::make_unique<grpc::ClientContext>();

      auto caller = new AsyncGrpcCaller<&PeerService::Stub::PrepareAsyncProbe>(
          std::move(stub), std::move(context),
          [this](EmptyResponse* /*response*/, grpc::Status* status) {
            if (status->ok()) {
              notifier_.Notify(kPortForwardedPeerProbeSuccess);
            } else {
              notifier_.Notify(kPortForwardedPeerProbeFailure);
            }
          });

      grpc_queue.Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));

      prober_.cond.wait_for(
          lock, std::chrono::seconds(1), [this] { return !prober_.enable; });

      if (!prober_.enable) break;
    }

    grpc_queue.Terminate();
  });
}

void
PeerManager::StopPortForwardedPeerProber()
{
  if (!port_forwarded_peer_prober_.joinable()) return;

  {
    std::lock_guard<std::mutex> lock(prober_.mtx);
    prober_.enable = false;
  }

  prober_.cond.notify_one();

  port_forwarded_peer_prober_.join();
}

void
PeerManager::PortForwardedPeerProbe(bool success)
{
  std::string host = kPortForwardedPeerHost;
  if (success) {
    for (auto& peer : peers_) {
      if (peer->host() == host) {
        peer->Ping();
        return;
      }
    }

    auto peer = CreatePeer(host, loop_, true);
    if (!peer) {
      LOG_ERROR("Failed to create a port forwarded peer");
      return;
    }

    peer->on_expired = [this, id = peer->id()]() {
      for (auto it = peers_.begin(); it != peers_.end();) {
        if ((*it)->id() == id) {
          it = peers_.erase(it);
          on_peer_lost(id);
        } else {
          it++;
        }
      }
    };

    peer->Ping();

    peers_.push_back(peer);

    on_peer_discover(peer->id());
  } else {
    for (auto it = peers_.begin(); it != peers_.end();) {
      if ((*it)->host() == host) {
        auto id = (*it)->id();
        it = peers_.erase(it);
        on_peer_lost(id);
      } else {
        it++;
      }
    }
  }
}

void
PeerManager::Notify(uint8_t signal)
{
  switch (signal) {
    case kPortForwardedPeerProbeSuccess:
      PortForwardedPeerProbe(true);
      break;

    case kPortForwardedPeerProbeFailure:
      PortForwardedPeerProbe(false);
      break;

    default:
      LOG_WARN("Unknown peer manager signal %u", signal);
      break;
  }
}

PeerManager::PeerManager(std::unique_ptr<ILoop> loop)
    : loop_(std::move(loop)), udp_socket_(io_context_), notifier_(loop_, this)
{
}

PeerManager::~PeerManager()
{
  if (udp_socket_source_) loop_->RemoveFd(udp_socket_source_.get());
  StopPortForwardedPeerProber();
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
