#include "server/channel.h"

#include "server/async-grpc-caller.h"
#include "server/session.h"

namespace zen::remote::server {

Channel::Channel(int control_fd, std::string host_port, uint64_t session_id,
    std::shared_ptr<SessionSerial> session_serial,
    uint32_t session_characteristics)
    : id_(session_serial->NextChannelSerial()),
      session_id_(session_id),
      session_characteristics_(session_characteristics),
      enabled_(true),
      control_fd_(control_fd),
      session_serial_(std::move(session_serial))
{
  grpc::ChannelArguments args;
  args.SetString(GRPC_ARG_OPTIMIZATION_TARGET, "latency");
  args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, 1000);
  args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 3000);
  args.SetInt(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0);
  args.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
  grpc_channel_ = grpc::CreateCustomChannel(
      host_port, grpc::InsecureChannelCredentials(), args);

  grpc_queue_.Start();

  job_queue_.StartWorkerThread();
}

Channel::~Channel() { Disable(); }

void
Channel::Disable()
{
  std::lock_guard<std::mutex> lock(mtx_);
  if (enabled_) {
    job_queue_.Terminate();
    grpc_queue_.Terminate();
    enabled_ = false;
  }
}

void
Channel::NotifyDisconnection()
{
  std::lock_guard<std::mutex> lock(mtx_);
  enum Session::ControlMessage message = Session::kDisconnect;
  if (enabled_) {
    write(control_fd_, &message, sizeof(message));
  }
}

void
Channel::PushJob(std::unique_ptr<IJob> job)
{
  job_queue_.Push(std::move(job));
}

void
Channel::PushGrpcCaller(std::unique_ptr<AsyncGrpcCallerBase> caller)
{
  grpc_queue_.Push(std::move(caller));
}

uint32_t
Channel::GetBusyness()
{
  return grpc_queue_.pending_count();
}

bool
Channel::wired()
{
  return session_characteristics_ & Session::kWired;
}

uint64_t
Channel::NewSerial(SerialType type)
{
  switch (type) {
    case kRequest: {
      std::lock_guard<std::mutex> lock(serial_mtx_);
      return serials_[type]++;
    }
    case kResource:
      return session_serial_->NextResourceSerial();

    default:
      return 0;
  }
}

std::shared_ptr<IChannel>
CreateChannel(std::shared_ptr<ISession>& session_base)
{
  auto session = std::dynamic_pointer_cast<Session>(session_base);
  auto channel =
      std::make_shared<Channel>(session->control_fd(), session->host_port(),
          session->id(), session->serial(), session->characteristics());
  session->AddChannel(channel);
  return channel;
}

}  // namespace zen::remote::server
