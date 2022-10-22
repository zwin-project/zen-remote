#pragma once

#include <zen-remote/server/remote.h>

namespace zen::remote::server {

struct IBuffer {
  virtual ~IBuffer() = default;

  virtual void *data() = 0;
};

/**
 * @param data is a pointer where the data is stored
 * @param on_release is an asynchronous callback that is called when the data is
 * no longer used by zen-remote. This callback is called by the loop associated
 * with `remote`
 */
std::unique_ptr<IBuffer> CreateBuffer(void *data,
    std::function<void()> on_release, std::shared_ptr<IRemote> remote);

}  // namespace zen::remote::server
