#pragma once

#include "core/common.h"
#include "zen-remote/loop.h"

namespace zen::remote {

class Context {
 public:
  DISABLE_MOVE_AND_COPY(Context);
  Context(std::unique_ptr<ILoop> loop) : loop_(std::move(loop)) {}

  inline std::unique_ptr<ILoop>& loop();
  inline boost::asio::io_context& io_context();

 private:
  std::unique_ptr<ILoop> loop_;
  boost::asio::io_context io_context_;
};

inline std::unique_ptr<ILoop>&
Context::loop()
{
  return loop_;
}

inline boost::asio::io_context&
Context::io_context()
{
  return io_context_;
}

}  // namespace zen::remote
