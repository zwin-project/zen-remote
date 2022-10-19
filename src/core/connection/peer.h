#pragma once

#include "core/common.h"
#include "core/context.h"
#include "zen-remote/loop.h"

namespace zen::remote::connection {

namespace ip = boost::asio::ip;

class Peer {
 public:
  /* The type of target peer node */
  enum class Target {
    kClient,
    kServer,
  };

  DISABLE_MOVE_AND_COPY(Peer);
  Peer() = delete;
  Peer(Target target, std::shared_ptr<Context> context);

  bool StartDiscover();

  inline ip::tcp::endpoint endpoint();
  inline std::shared_ptr<grpc::Channel> grpc_channel();

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

  std::shared_ptr<grpc::Channel> grpc_channel_;  // used only by the server
};

inline ip::tcp::endpoint
Peer::endpoint()
{
  return endpoint_;
}

inline std::shared_ptr<grpc::Channel>
Peer::grpc_channel()
{
  if (!grpc_channel_) {
    auto host_port =
        endpoint_.address().to_string() + ":" + std::to_string(kGrpcPort);
    grpc_channel_ =
        grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials());
  }

  return grpc_channel_;
}

}  // namespace zen::remote::connection
