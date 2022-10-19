#pragma once

namespace zen::remote::client {

struct IResource {
  virtual ~IResource() = default;

  virtual uint64_t id() = 0;
};

}  // namespace zen::remote::client
