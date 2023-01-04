#pragma once

#include <zen-remote/server/channel.h>

#include <memory>

namespace zen::remote::server {

struct IGlSampler {
  virtual ~IGlSampler() = default;

  virtual void GlSamplerParameterf(uint32_t pname, float param) = 0;

  virtual void GlSamplerParameteri(uint32_t pname, int32_t param) = 0;

  virtual void GlSamplerParameterfv(uint32_t pname, const float *params) = 0;

  virtual void GlSamplerParameteriv(uint32_t pname, const int32_t *params) = 0;

  virtual void GlSamplerParameterIiv(uint32_t pname, const int32_t *params) = 0;

  virtual void GlSamplerParameterIuiv(
      uint32_t pname, const uint32_t *params) = 0;

  virtual uint64_t id() = 0;
};

std::unique_ptr<IGlSampler> CreateGlSampler(std::shared_ptr<IChannel> channel);

}  // namespace zen::remote::server
