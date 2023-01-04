#pragma once

#include "core/common.h"
#include "zen-remote/server/gl-sampler.h"

namespace zen::remote::server {

class Channel;

class GlSampler final : public IGlSampler {
 public:
  DISABLE_MOVE_AND_COPY(GlSampler);
  GlSampler() = delete;
  GlSampler(std::shared_ptr<Channel> channel);
  ~GlSampler();

  void Init();

  void GlSamplerParameterf(uint32_t pname, float param) override;

  void GlSamplerParameteri(uint32_t pname, int32_t param) override;

  void GlSamplerParameterfv(uint32_t pname, const float *params) override;

  void GlSamplerParameteriv(uint32_t pname, const int32_t *params) override;

  void GlSamplerParameterIiv(uint32_t pname, const int32_t *params) override;

  void GlSamplerParameterIuiv(uint32_t pname, const uint32_t *params) override;

  uint64_t id() override;

 private:
  uint64_t id_;
  std::weak_ptr<Channel> channel_;
};

}  // namespace zen::remote::server
