#include "discovery-broadcast.h"

#include "core/connection/packet.h"
#include "core/logger.h"

namespace zen::remote::client {

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

DiscoveryBroadcasts::~DiscoveryBroadcasts() { StopAndWait(); }

void
DiscoveryBroadcasts::StartIfNotRunning()
{
  std::lock_guard<std::mutex> lock_thread(thread_mutex_);
  std::lock_guard<std::mutex> lock(mutex_);

  if (running_) return;

  running_ = true;

  thread_ = std::thread([this] {
    boost::system::error_code err;
    asio::io_context io_context;
    ip::udp::socket udp_socket(io_context);
    ip::udp::endpoint broadcast_endpoint(
        ip::address_v4::broadcast(), kDiscoverPort);
    connection::DiscoverPacket packet;

    for (;;) {
      udp_socket.open(ip::udp::v4(), err);

      if (err) {
        LOG_WARN("Failed to open UDP socket: %s", err.what().c_str());
        LOG_WARN("Retry in 5 sec...");
        std::unique_lock<std::mutex> lock(mutex_);

        cond_.wait_for(lock, std::chrono::seconds(5),
            [this] { return running_ == false; });

        if (running_ == false) return;
      } else {
        break;
      }
    }

    udp_socket.set_option(ip::udp::socket::reuse_address(true));
    udp_socket.set_option(asio::socket_base::broadcast(true));

    for (;;) {
      udp_socket.send_to(boost::asio::buffer(&packet, sizeof(packet)),
          broadcast_endpoint, 0, err);

      if (err) {
        LOG_WARN("Failed to send UDP broadcast packet for client discovery: %s",
            err.what().c_str());
        LOG_WARN("Retry in 5 sec...");
        std::unique_lock<std::mutex> lock(mutex_);

        cond_.wait_for(lock, std::chrono::seconds(5),
            [this] { return running_ == false; });

        if (running_ == false) return;

        continue;
      }

      LOG_DEBUG("Sent UPD broadcast packet for client discovery");

      {
        std::unique_lock<std::mutex> lock(mutex_);

        cond_.wait_for(lock, std::chrono::seconds(1),
            [this] { return running_ == false; });

        if (running_ == false) return;
      }
    }
  });
}

void
DiscoveryBroadcasts::StopAndWait()
{
  std::lock_guard<std::mutex> lock(thread_mutex_);
  {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!running_) return;

    running_ = false;
  }

  cond_.notify_one();

  thread_.join();
}

}  // namespace zen::remote::client
