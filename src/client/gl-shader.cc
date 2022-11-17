#include "client/gl-shader.h"

#include "client/atomic-command-queue.h"
#include "core/logger.h"

namespace zen::remote::client {

GlShader::GlShader(uint64_t id, std::string source, uint32_t type,
    AtomicCommandQueue *update_rendering_queue)
    : id_(id),
      source_(std::move(source)),
      type_(type),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

GlShader::~GlShader()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {
    if (rendering->shader_id != 0) {
      glDeleteShader(rendering->shader_id);
    }
  });

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
GlShader::Commit()
{
  if (committed_) return;
  committed_ = true;

  auto command = CreateCommand([type = type_, source = std::move(source_),
                                   rendering = rendering_](bool cancel) {
    if (cancel) {
      return;
    }

    if (rendering->shader_id != 0) return;

    auto shader_id = glCreateShader(type);
    const char *source_cstr = source.c_str();
    glShaderSource(shader_id, 1, &source_cstr, NULL);
    glCompileShader(shader_id);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
      int log_length = 0;
      glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
      std::string log_message(log_length, ' ');
      glGetShaderInfoLog(shader_id, log_length, NULL, log_message.data());
      LOG_ERROR("Failed to compile %s shader: %s",
          type == GL_VERTEX_SHADER     ? "vertex"
          : type == GL_FRAGMENT_SHADER ? "fragment"
                                       : "unknown",
          log_message.c_str());
      return;
    }

    rendering->shader_id = shader_id;
  });

  update_rendering_queue_->Push(std::move(command));
}

uint64_t
GlShader::id()
{
  return id_;
}

}  // namespace zen::remote::client
