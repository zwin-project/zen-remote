#include "client/gl-sampler.h"

#include "client/atomic-command-queue.h"
#include "core/logger.h"

namespace zen::remote::client {

GlSampler::GlSampler(uint64_t id, AtomicCommandQueue *update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

GlSampler::ParameterCommand::ParameterCommand(enum ParameterType type,
    uint32_t pname, float float_param, int32_t int_param,
    const std::string vector_param)
    : type(type),
      pname(pname),
      float_param(float_param),
      int_param(int_param),
      vector_param(std::move(vector_param))
{
}

GlSampler::~GlSampler()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {
    if (rendering->sampler_id != 0) {
      glDeleteSamplers(1, &rendering->sampler_id);
    }
  });

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
GlSampler::Commit()
{
  auto command = CreateCommand([rendering = rendering_](bool cancel) {
    if (cancel) {
      return;
    }

    if (rendering->sampler_id == 0) {
      glGenSamplers(1, &rendering->sampler_id);
    }
  });

  update_rendering_queue_->Push(std::move(command));

  if (pending_.parameter_commands.empty()) return;

  std::list<ParameterCommand> parameter_commands;
  pending_.parameter_commands.swap(parameter_commands);

  command = CreateCommand(
      [rendering = rendering_,
          parameter_commands = std::move(parameter_commands)](bool cancel) {
        if (cancel) {
          return;
        }

        for (auto &command : parameter_commands) {
          switch (command.type) {
            case ParameterType::f:
              glSamplerParameterf(
                  rendering->sampler_id, command.pname, command.float_param);
              break;

            case ParameterType::i:
              glSamplerParameteri(
                  rendering->sampler_id, command.pname, command.int_param);
              break;

            case ParameterType::fv:
              glSamplerParameterfv(rendering->sampler_id, command.pname,
                  (GLfloat *)command.vector_param.data());
              break;

            case ParameterType::iv:
              glSamplerParameteriv(rendering->sampler_id, command.pname,
                  (GLint *)command.vector_param.data());
              break;

            case ParameterType::Iiv:
              glSamplerParameterIiv(rendering->sampler_id, command.pname,
                  (GLint *)command.vector_param.data());
              break;

            case ParameterType::Iuiv:
              glSamplerParameterIuiv(rendering->sampler_id, command.pname,
                  (GLuint *)command.vector_param.data());
              break;
          }
        }
      });

  update_rendering_queue_->Push(std::move(command));
}

void
GlSampler::Parameterf(uint32_t pname, float param)
{
  pending_.parameter_commands.emplace_back(
      ParameterType::f, pname, param, 0, "");
}

void
GlSampler::Parameteri(uint32_t pname, int32_t param)
{
  pending_.parameter_commands.emplace_back(
      ParameterType::i, pname, 0, param, "");
}

void
GlSampler::Parameterfv(uint32_t pname, const std::string &params)
{
  pending_.parameter_commands.emplace_back(
      ParameterType::fv, pname, 0, 0, params);
}

void
GlSampler::Parameteriv(uint32_t pname, const std::string &params)
{
  pending_.parameter_commands.emplace_back(
      ParameterType::iv, pname, 0, 0, params);
}

void
GlSampler::ParameterIiv(uint32_t pname, const std::string &params)
{
  pending_.parameter_commands.emplace_back(
      ParameterType::Iiv, pname, 0, 0, params);
}

void
GlSampler::ParameterIuiv(uint32_t pname, const std::string &params)
{
  pending_.parameter_commands.emplace_back(
      ParameterType::Iuiv, pname, 0, 0, params);
}

uint64_t
GlSampler::id()
{
  return id_;
}

}  // namespace zen::remote::client
