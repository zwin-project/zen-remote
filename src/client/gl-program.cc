#include "client/gl-program.h"

#include "client/atomic-command-queue.h"
#include "client/gl-shader.h"
#include "core/logger.h"

namespace zen::remote::client {

GlProgram::GlProgram(uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState)
{
}

GlProgram::~GlProgram()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {
    if (rendering->program_id != 0) {
      glDeleteProgram(rendering->program_id);
    }
  });

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
GlProgram::Commit()
{
  if (pending_.should_link == false) return;

  std::vector<std::weak_ptr<GlShader>> gl_shaders;

  pending_.gl_shaders.swap(gl_shaders);

  for (auto& shader_ptr : gl_shaders) {
    if (auto shader = shader_ptr.lock()) {
      shader->Commit();
    }
  }

  auto command =
      CreateCommand([rendering = rendering_, gl_shaders](bool cancel) {
        if (cancel) {
          return;
        }

        if (rendering->program_id != 0) return;

        auto program_id = glCreateProgram();

        for (auto& shader_ptr : gl_shaders) {
          if (auto shader = shader_ptr.lock()) {
            glAttachShader(program_id, shader->shader_id());
          }
        }

        glLinkProgram(program_id);

        GLint linked = GL_FALSE;
        glGetProgramiv(program_id, GL_LINK_STATUS, &linked);

        if (linked != GL_TRUE) {
          int log_length = 0;
          glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);
          std::string log_message(log_length, ' ');
          glGetProgramInfoLog(program_id, log_length, NULL, log_message.data());
          LOG_ERROR("Fail to link program: %s", log_message.c_str());
          return;
        }

        rendering->program_id = program_id;
      });

  update_rendering_queue_->Push(std::move(command));

  pending_.should_link = false;
}

void
GlProgram::GlAttachShader(std::weak_ptr<GlShader> gl_shader)
{
  pending_.gl_shaders.push_back(gl_shader);
}

void
GlProgram::GlLinkProgram()
{
  pending_.should_link = true;
}

uint64_t
GlProgram::id()
{
  return id_;
}

}  // namespace zen::remote::client
