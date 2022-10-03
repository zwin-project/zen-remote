#pragma once

#include "core/common.h"
#include "core/context.h"
#include "zen/display-system/remote/core/loop.h"

namespace zen::display_system::remote::connection {

namespace ip = boost::asio::ip;

class Peer {
 public:
  /* The type of target peer node */
  enum class Target {
    kClient,
    kServer,
  };

  DISABLE_MOVE_AND_COPY(Peer);
  Peer(Target target, std::shared_ptr<Context> context)
      : target_(target),
        context_(std::move(context)),
        udp_socket_(context_->io_context()),
        tcp_socket_(context_->io_context()),
        tcp_acceptor_(context_->io_context()),
        udp_socket_source_(std::make_unique<FdSource>()),
        tcp_socket_source_(std::make_unique<FdSource>()),
        tcp_acceptor_source_(std::make_unique<FdSource>()){};
  ~Peer() = default;

  bool StartDiscover();

  inline ip::tcp::endpoint endpoint();

  struct {
    boost::signals2::signal<void()> discoverd;
  } signals;

 private:
  enum class Status {
    kInitial,
    kDiscovering,
    kDiscovered,
  };

  bool DiscoverServer();
  void AcceptTcpSocket(int fd, uint32_t mask);
  void ReadTcpSocket(int fd, uint32_t mask);

  bool DiscoverClient();
  void AcceptDiscoverBroadCast(int fd, uint32_t mask);

  Target target_;
  std::shared_ptr<Context> context_;
  ip::tcp::endpoint endpoint_;

  // Readonly from discovered_broadcast_thread_;
  Status status_ = Status::kInitial;

  ip::udp::socket udp_socket_;
  ip::tcp::socket tcp_socket_;
  ip::tcp::acceptor tcp_acceptor_;
  std::unique_ptr<FdSource> udp_socket_source_;
  std::unique_ptr<FdSource> tcp_socket_source_;
  std::unique_ptr<FdSource> tcp_acceptor_source_;
  boost::asio::streambuf tcp_read_buf_;

  std::thread discover_broadcast_thread_;
};

inline ip::tcp::endpoint
Peer::endpoint()
{
  return endpoint_;
}

}  // namespace zen::display_system::remote::connection
