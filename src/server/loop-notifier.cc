#include "server/loop-notifier.h"

#include "core/logger.h"

namespace zen::remote::server {

LoopNotifier::LoopNotifier(
    std::shared_ptr<ILoop> loop, ILoopNotifierDelegate *delegate)
    : pipe_{0, 0}, loop_(loop), delegate_(delegate)
{
}

LoopNotifier::~LoopNotifier()
{
  if (pipe_[0] != 0) {
    close(pipe_[0]);
    close(pipe_[1]);
  }

  if (event_source_) {
    loop_->RemoveFd(event_source_.get());
  }
}

void
LoopNotifier::Notify(uint8_t signal)
{
  write(pipe_[1], &signal, sizeof(signal));
}

void
LoopNotifier::Callback(int fd, uint32_t /*mask*/)
{
  uint8_t signal;
  ssize_t result = read(fd, &signal, sizeof(signal));

  if (result <= 0) {
    LOG_ERROR("Failed to read from pipe");
    return;
  }

  delegate_->Notify(signal);
}

bool
LoopNotifier::Init()
{
  if (pipe2(pipe_, O_CLOEXEC | O_NONBLOCK) == -1) {
    LOG_ERROR("Failed to create a pipe");
    return false;
  }

  event_source_ = std::make_unique<FdSource>();
  event_source_->fd = pipe_[0];
  event_source_->mask = FdSource::kReadable;
  event_source_->callback = std::bind(&LoopNotifier::Callback, this,
      std::placeholders::_1, std::placeholders::_2);

  loop_->AddFd(event_source_.get());

  return true;
}

}  // namespace zen::remote::server
