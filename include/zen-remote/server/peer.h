#pragma once

#include <string>

namespace zen::remote::server {

struct IPeer {
  virtual ~IPeer() = default;

  virtual std::string host() = 0;
  virtual bool wired() = 0;
  virtual uint64_t id() = 0;
};

}  // namespace zen::remote::server
