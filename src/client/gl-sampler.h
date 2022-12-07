#pragma once

#include "client/resource.h"
#include "core/common.h"

namespace zen::remote::client {

class AtomicCommandQueue;

class GlSampler final : public IResource {
  struct RenderingState {
    GLuint sampler_id = 0;
  };

  enum class ParameterType {
    f,
    i,
    fv,
    iv,
    Iiv,
    Iuiv,
  };

  struct ParameterCommand {
    ParameterCommand(enum ParameterType type, uint32_t pname, float float_param,
        int32_t int_param, const std::string vector_param);

    enum ParameterType type;
    uint32_t pname;
    float float_param;
    int32_t int_param;
    const std::string vector_param;
  };

 public:
  DISABLE_MOVE_AND_COPY(GlSampler);
  GlSampler() = delete;
  GlSampler(uint64_t id, AtomicCommandQueue *update_rendering_queue);
  ~GlSampler();

  /** Used in the update thread */
  void Commit();

  /** Used in the update thread */
  void Parameterf(uint32_t pname, float param);

  /** Used in the update thread */
  void Parameteri(uint32_t pname, int32_t param);

  /** Used in the update thread */
  void Parameterfv(uint32_t pname, const std::string &params);

  /** Used in the update thread */
  void Parameteriv(uint32_t pname, const std::string &params);

  /** Used in the update thread */
  void ParameterIiv(uint32_t pname, const std::string &params);

  /** Used in the update thread */
  void ParameterIuiv(uint32_t pname, const std::string &params);

  uint64_t id() override;

  /** Used in the rendering thread */
  inline GLuint sampler_id();

 private:
  const uint64_t id_;
  AtomicCommandQueue *update_rendering_queue_;

  struct {
    std::list<ParameterCommand> parameter_commands;
  } pending_;

  std::shared_ptr<RenderingState> rendering_;
};

inline GLuint
GlSampler::sampler_id()
{
  return rendering_->sampler_id;
}

}  // namespace zen::remote::client
