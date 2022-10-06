#pragma once

#include "core/connection/peer.h"
#include "core/context.h"
#include "zen-remote/remote.h"

namespace zen::remote {

class Remote : public IRemote {
 public:
  Remote(std::unique_ptr<ILoop> loop)
      : context_(std::make_unique<Context>(std::move(loop)))
  {
  }

  void Start() override;

  void Stop() override;

 private:
  std::unique_ptr<connection::Peer> peer_;
  std::shared_ptr<Context> context_;
};

}  // namespace zen::remote
