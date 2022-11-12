#include "client/session-manager.h"

#include "client/session.h"
#include "core/connection/packet.h"
#include "core/logger.h"

namespace zen::remote::client {

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

SessionManager::~SessionManager()
{
  std::lock_guard<std::mutex> thread_lock(broadcast_.thread_mutex);
  StopDiscoverBroadcast();
}

bool
SessionManager::Start()
{
  std::lock_guard<std::mutex> thread_lock(broadcast_.thread_mutex);
  StartDiscoverBroadcast();
  return true;
}

uint64_t
SessionManager::ResetCurrent()
{
  uint64_t id;
  {
    std::lock_guard<std::mutex> lock(current_mutex_);
    current_ = std::make_unique<Session>();
    id = current_->id();
  }

  {
    std::lock_guard<std::mutex> thread_lock(broadcast_.thread_mutex);
    StopDiscoverBroadcast();
  }
  return id;
}

void
SessionManager::ClearCurrent()
{
  {
    std::lock_guard<std::mutex> lock(current_mutex_);
    if (!current_) return;
    current_.reset();
  }

  {
    std::lock_guard<std::mutex> lock(broadcast_.thread_mutex);
    StartDiscoverBroadcast();
  }
}

std::shared_ptr<ResourcePool>
SessionManager::GetCurrentResourcePool()
{
  std::lock_guard<std::mutex> lock(current_mutex_);
  if (current_) return current_->pool();
  return std::shared_ptr<ResourcePool>();
}

void
SessionManager::StartDiscoverBroadcast()
{
  if (broadcast_.thread.joinable()) {
    StopDiscoverBroadcast();
  }

  broadcast_.running = true;

  broadcast_.thread = std::thread([this] {
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
        std::this_thread::sleep_for(std::chrono::seconds(5));
        continue;
      }

      break;
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
        std::this_thread::sleep_for(std::chrono::seconds(5));
        continue;
      }

      LOG_DEBUG("Sent UPD broadcast packet for client discovery");

      {
        std::unique_lock<std::mutex> lock(broadcast_.mutex);

        broadcast_.cond.wait_for(lock, std::chrono::seconds(1),
            [this]() { return broadcast_.running == false; });

        if (broadcast_.running == false) break;
      }
    }
  });
}

void
SessionManager::StopDiscoverBroadcast()
{
  if (!broadcast_.thread.joinable()) return;

  {
    std::lock_guard<std::mutex> lock(broadcast_.mutex);
    broadcast_.running = false;
  }

  broadcast_.cond.notify_one();

  broadcast_.thread.join();
}

}  // namespace zen::remote::client
