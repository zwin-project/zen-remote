#include "core/connection/peer.h"

#include "core/logger.h"

namespace zen::remote::connection {

namespace {

constexpr int kDiscoverPort = 9983;
#define DISCOVER_PACKET_PREAMBLE "55d65470abe34d2ca21c21e4eb1033d5"  // UUID
#define DISCOVER_PACKET_VERSION 1

struct DiscoverPacket {
  DiscoverPacket()
  {
    memcpy(preamble, DISCOVER_PACKET_PREAMBLE, sizeof(preamble));
  }

  uint8_t preamble[32];
  uint16_t version = DISCOVER_PACKET_VERSION;
} __attribute__((__packed__));

}  // namespace

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

bool
Peer::StartDiscover()
{
  if (target_ == Target::kClient) {
    return DiscoverClient();
  } else {
    return DiscoverServer();
  }
}

bool
Peer::DiscoverServer()
{
  if (status_ != Status::kInitial) return true;
  status_ = Status::kDiscovering;

  discover_broadcast_thread_ = std::thread([this] {
    boost::system::error_code err;
    DiscoverPacket packet;

    asio::io_context io_context;
    ip::udp::socket udp_socket(io_context);
    ip::udp::endpoint broadcast_endpoint(
        ip::address_v4::broadcast(), kDiscoverPort);

    for (;;) {
      udp_socket_.open(ip::udp::v4(), err);

      if (err) {
        LOG_WARN("Failed to open UDP socket. Retry in 5 sec...");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        continue;
      }

      break;
    }

    udp_socket_.set_option(ip::udp::socket::reuse_address(true));
    udp_socket_.set_option(asio::socket_base::broadcast(true));

    for (;;) {
      udp_socket_.send_to(boost::asio::buffer(&packet, sizeof(packet)),
          broadcast_endpoint, 0, err);

      if (err) {
        LOG_WARN(
            "Failed to send UDP broadcast packet for client discovery. "
            "Retry in 5 sec...");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        continue;
      }

      LOG_DEBUG("Sent UPD broadcast packet for client discovery");

      for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (status_ != Status::kDiscovering) return;
      }
    }
  });

  boost::system::error_code err;

  tcp_acceptor_.open(ip::tcp::v4(), err);
  if (err) {
    LOG_ERROR("Failed to open tcp acceptor of IPv4: %s", err.what().c_str());
    return false;
  }

  tcp_acceptor_.bind(ip::tcp::endpoint(ip::tcp::v4(), kDiscoverPort), err);
  if (err) {
    LOG_ERROR("Failed to bind tcp acceptor to port %d: %s", kDiscoverPort,
        err.what().c_str());
    return false;
  }

  tcp_acceptor_.listen(asio::socket_base::max_listen_connections, err);
  if (err) {
    LOG_ERROR("Failed to start listening port %d: %s", kDiscoverPort,
        err.what().c_str());
    return false;
  }

  tcp_acceptor_source_->fd = tcp_acceptor_.native_handle();
  tcp_acceptor_source_->mask =
      FdSource::kReadable | FdSource::kHangup | FdSource::kError;
  tcp_acceptor_source_->callback = std::bind(&Peer::AcceptTcpSocket, this,
      std::placeholders::_1, std::placeholders::_2);

  auto& loop = context_->loop();
  loop->AddFd(tcp_acceptor_source_.get());

  return true;
}

void
Peer::AcceptTcpSocket(int /*fd*/, uint32_t /*mask*/)
{
  boost::system::error_code err;
  tcp_acceptor_.accept(tcp_socket_, err);
  if (err) {
    LOG_WARN("Failed to accept: %s", err.what().c_str());
    tcp_socket_ = ip::tcp::socket(context_->io_context());
    return;
  }

  tcp_socket_source_->fd = tcp_socket_.native_handle();
  tcp_socket_source_->mask =
      FdSource::kReadable | FdSource::kHangup | FdSource::kError;
  tcp_socket_source_->callback = std::bind(
      &Peer::ReadTcpSocket, this, std::placeholders::_1, std::placeholders::_2);

  auto& loop = context_->loop();
  loop->AddFd(tcp_socket_source_.get());

  return;
}

void
Peer::ReadTcpSocket(int /*fd*/, uint32_t /*mask*/)
{
  boost::system::error_code err;

  asio::read(tcp_socket_, tcp_read_buf_,
      asio::transfer_exactly(sizeof(DiscoverPacket)), err);
  if (err) {
    LOG_WARN("Failed to read from tcp socket: %s", err.what().c_str());
    tcp_socket_.close();
    tcp_socket_ = ip::tcp::socket(context_->io_context());
    return;
  }

  auto packet = asio::buffer_cast<const DiscoverPacket*>(tcp_read_buf_.data());

  if (memcmp(packet->preamble, DISCOVER_PACKET_PREAMBLE,
          sizeof(packet->preamble)) != 0) {
    LOG_WARN("Unexpected tcp packet received");
    tcp_socket_.close();
    tcp_socket_ = ip::tcp::socket(context_->io_context());
    return;
  }

  if (packet->version != DISCOVER_PACKET_VERSION) {
    LOG_WARN("Server version mismatch");
    tcp_socket_.close();
    tcp_socket_ = ip::tcp::socket(context_->io_context());
    return;
  }

  status_ = Status::kDiscovered;
  if (discover_broadcast_thread_.joinable()) discover_broadcast_thread_.join();
  endpoint_ = tcp_socket_.remote_endpoint();

  auto& loop = context_->loop();
  loop->RemoveFd(tcp_acceptor_source_.get());
  loop->RemoveFd(tcp_socket_source_.get());
  tcp_acceptor_.close();
  tcp_socket_.close();

  signals.discoverd();
}

bool
Peer::DiscoverClient()
{
  boost::system::error_code err;
  if (status_ != Status::kInitial) return true;
  status_ = Status::kDiscovering;

  udp_socket_.open(ip::udp::v4(), err);
  if (err) {
    LOG_ERROR("Failed to open UDP socket: %s", err.what().c_str());
    udp_socket_ = ip::udp::socket(context_->io_context());
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
    udp_socket_ = ip::udp::socket(context_->io_context());
    return false;
  }

  udp_socket_source_->fd = udp_socket_.native_handle();
  udp_socket_source_->mask =
      FdSource::kReadable | FdSource::kHangup | FdSource::kError;
  udp_socket_source_->callback = std::bind(&Peer::AcceptDiscoverBroadCast, this,
      std::placeholders::_1, std::placeholders::_2);

  auto& loop = context_->loop();
  loop->AddFd(udp_socket_source_.get());

  return true;
}

void
Peer::AcceptDiscoverBroadCast(int /*fd*/, uint32_t /*mask*/)
{
  boost::system::error_code err;
  ip::udp::endpoint client_endpoint;
  DiscoverPacket packet;

  size_t len = udp_socket_.receive_from(
      boost::asio::buffer(&packet, sizeof(packet)), client_endpoint, 0, err);
  if (err) {
    LOG_ERROR("Failed to read from UDP socket");
    return;
  }

  if (len != sizeof(DiscoverPacket)) {
    LOG_WARN("Unexpected broadcast length");
    return;
  }

  if (memcmp(DISCOVER_PACKET_PREAMBLE, packet.preamble,
          sizeof(packet.preamble)) != 0) {
    LOG_WARN("Unexpected broadcast packet received");
    return;
  }

  if (packet.version != DISCOVER_PACKET_VERSION) {
    LOG_WARN("Client version mismatch");
    return;
  }

  auto& loop = context_->loop();
  loop->RemoveFd(udp_socket_source_.get());

  tcp_socket_.connect(
      ip::tcp::endpoint(client_endpoint.address(), kDiscoverPort));

  asio::write(tcp_socket_, asio::buffer(&packet, sizeof(packet)), err);
  if (err) {
    LOG_WARN("Failed to write to socket");
    tcp_socket_.close();
    tcp_socket_ = ip::tcp::socket(context_->io_context());
    return;
  }

  status_ = Status::kDiscovered;
  endpoint_ = tcp_socket_.remote_endpoint();

  tcp_socket_.close();

  signals.discoverd();
}

}  // namespace zen::remote::connection
