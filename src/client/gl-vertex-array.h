#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

class GlVertexArray final : public IResource {
 public:
  DISABLE_MOVE_AND_COPY(GlVertexArray);
  GlVertexArray() = delete;
  GlVertexArray(uint64_t id, AtomicCommandQueue *update_rendering_queue);

  void Commit();

  uint64_t id() override;

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;
};

}  // namespace zen::remote::client
