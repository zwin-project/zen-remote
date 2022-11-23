#include "client/gl-base-technique.h"

#include "client/atomic-command-queue.h"
#include "client/gl-program.h"
#include "client/gl-texture.h"
#include "client/gl-vertex-array.h"
#include "zen-remote/client/camera.h"

namespace zen::remote::client {

GlBaseTechnique::GlBaseTechnique(
    uint64_t id, AtomicCommandQueue* update_rendering_queue)
    : id_(id),
      update_rendering_queue_(update_rendering_queue),
      rendering_(new RenderingState())
{
}

GlBaseTechnique::~GlBaseTechnique()
{
  auto command = CreateCommand([rendering = rendering_](bool /*cancel*/) {});

  rendering_.reset();

  update_rendering_queue_->Push(std::move(command));
}

void
GlBaseTechnique::Commit()
{
  if (auto program = pending_.program.lock()) {
    program->Commit();
  }

  if (auto vertex_array = pending_.vertex_array.lock()) {
    vertex_array->Commit();
  }

  if (pending_.draw_method_damaged) {
    pending_.draw_method_damaged = false;
    auto command =
        CreateCommand([args = pending_.draw_args, method = pending_.draw_method,
                          rendering = rendering_](bool cancel) {
          if (cancel) {
            return;
          }

          rendering->draw_args = args;
          rendering->draw_method = method;
        });

    update_rendering_queue_->Push(std::move(command));
  }

  if (pending_.program_damaged) {
    pending_.program_damaged = false;
    auto command = CreateCommand(
        [program = pending_.program, rendering = rendering_](bool cancel) {
          if (cancel) {
            return;
          }

          rendering->program = program;
        });

    update_rendering_queue_->Push(std::move(command));
  }

  if (pending_.vertex_array_damaged) {
    pending_.vertex_array_damaged = false;
    auto command = CreateCommand([vertex_array = pending_.vertex_array,
                                     rendering = rendering_](bool cancel) {
      if (cancel) {
        return;
      }

      rendering->vertex_array = vertex_array;
    });

    update_rendering_queue_->Push(std::move(command));
  }

  if (!pending_.gl_textures.empty()) {
    std::list<std::pair<uint32_t, std::weak_ptr<GlTexture>>> gl_textures;
    pending_.gl_textures.swap(gl_textures);
    auto command = CreateCommand([gl_textures = std::move(gl_textures),
                                     rendering = rendering_](bool cancel) {
      if (cancel) {
        return;
      }

      for (auto& texture : gl_textures) {
        rendering->gl_textures.push_back(texture);
      }
    });

    update_rendering_queue_->Push(std::move(command));
  }

  if (!pending_.program.expired()) {
    std::list<UniformVariable> uniform_variables;
    pending_.uniform_variables.swap(uniform_variables);
    auto command =
        CreateCommand([uniform_variables = std::move(uniform_variables),
                          rendering = rendering_](bool cancel) {
          if (cancel) {
            return;
          }

          auto program = rendering->program.lock();
          if (!program || program->program_id() == 0) {
            return;
          }

          for (auto& uniform_variable : uniform_variables) {
            int32_t location = uniform_variable.location;

            if (!uniform_variable.name.empty()) {
              location = glGetUniformLocation(
                  program->program_id(), uniform_variable.name.c_str());
            }

            if (location < 0) continue;

            auto [iterator, newly_inserted] =
                rendering->uniform_variables.insert(
                    std::pair<uint32_t, UniformVariable>{location, {}});

            if (newly_inserted) {
              (*iterator).second.location = location;
            }

            (*iterator).second.type = uniform_variable.type;
            (*iterator).second.col = uniform_variable.col;
            (*iterator).second.row = uniform_variable.row;
            (*iterator).second.count = uniform_variable.count;
            (*iterator).second.transpose = uniform_variable.transpose;
            (*iterator).second.value = std::move(uniform_variable.value);
          }
        });

    update_rendering_queue_->Push(std::move(command));
  }
}

void
GlBaseTechnique::Bind(std::weak_ptr<GlProgram> program)
{
  pending_.program = program;
  pending_.program_damaged = true;
}

void
GlBaseTechnique::Bind(std::weak_ptr<GlVertexArray> vertex_array)
{
  pending_.vertex_array = vertex_array;
  pending_.vertex_array_damaged = true;
}

void
GlBaseTechnique::Bind(std::weak_ptr<GlTexture> texture, uint32_t target)
{
  pending_.gl_textures.push_back(std::make_pair(target, texture));
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  pending_.draw_method = DrawMethod::kArrays;
  pending_.draw_args.arrays.mode = mode;
  pending_.draw_args.arrays.count = count;
  pending_.draw_args.arrays.first = first;
  pending_.draw_method_damaged = true;
}

void
GlBaseTechnique::GlUniform(uint32_t location, std::string name,
    UniformVariableType type, uint32_t col, uint32_t row, uint32_t count,
    bool transpose, std::string value)
{
  if (!(0 < col && col <= 4 && 0 < row && row <= 4)) return;

  auto& uniform_variable = pending_.uniform_variables.emplace_back();
  uniform_variable.location = location;
  uniform_variable.name = name;
  uniform_variable.type = type;
  uniform_variable.col = col;
  uniform_variable.row = row;
  uniform_variable.count = count;
  uniform_variable.transpose = transpose;
  uniform_variable.value = std::move(value);
}

void
GlBaseTechnique::ApplyUniformVariables(
    GLuint program_id, Camera* camera, const glm::mat4& model)
{
  glm::mat4 vp;
  std::memcpy(&vp, &camera->vp, sizeof(glm::mat4));
  glm::mat4 mvp = vp * model;

  auto mvp_location = glGetUniformLocation(program_id, "mvp");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

  static void (*uniform_matrix[3][3])(GLint location, GLsizei count,
      GLboolean transpose, const GLfloat* value) = {
      {glUniformMatrix2fv, glUniformMatrix2x3fv, glUniformMatrix2x4fv},
      {glUniformMatrix3x2fv, glUniformMatrix3fv, glUniformMatrix3x4fv},
      {glUniformMatrix4x2fv, glUniformMatrix4x3fv, glUniformMatrix4fv},
  };

  for (auto& [location, uniform] : rendering_->uniform_variables) {
    if (uniform.col == 1) {
      if (uniform.type == UNIFORM_VARIABLE_TYPE_INT) {
        auto data = (GLint*)uniform.value.data();
        if (uniform.row == 1) glUniform1iv(location, uniform.count, data);
        if (uniform.row == 2) glUniform2iv(location, uniform.count, data);
        if (uniform.row == 3) glUniform3iv(location, uniform.count, data);
        if (uniform.row == 4) glUniform4iv(location, uniform.count, data);
      } else if (uniform.type == UNIFORM_VARIABLE_TYPE_UINT) {
        auto data = (GLuint*)uniform.value.data();
        if (uniform.row == 1) glUniform1uiv(location, uniform.count, data);
        if (uniform.row == 2) glUniform2uiv(location, uniform.count, data);
        if (uniform.row == 3) glUniform3uiv(location, uniform.count, data);
        if (uniform.row == 4) glUniform4uiv(location, uniform.count, data);
      } else if (uniform.type == UNIFORM_VARIABLE_TYPE_FLOAT) {
        auto data = (GLfloat*)uniform.value.data();
        if (uniform.row == 1) glUniform1fv(location, uniform.count, data);
        if (uniform.row == 2) glUniform2fv(location, uniform.count, data);
        if (uniform.row == 3) glUniform3fv(location, uniform.count, data);
        if (uniform.row == 4) glUniform4fv(location, uniform.count, data);
      }
    } else {
      auto data = (GLfloat*)uniform.value.data();
      uniform_matrix[uniform.col - 1][uniform.row - 1](
          uniform.location, uniform.count, uniform.transpose, data);
    }
  }
}

void
GlBaseTechnique::ApplyGlTexture()
{
  for (auto& texture : rendering_->gl_textures) {
    if (auto gl_texture = texture.second.lock()) {
      glBindTexture(texture.first, gl_texture->texture_id());
    }
  }
}

void
GlBaseTechnique::UnapplyGlTextures()
{
  for (auto& texture : rendering_->gl_textures) {
    glBindTexture(texture.first, 0);
  }
}

void
GlBaseTechnique::Render(Camera* camera, const glm::mat4& model)
{
  switch (rendering_->draw_method) {
    case DrawMethod::kNone:
      break;

    case DrawMethod::kArrays: {
      auto vertex_array = rendering_->vertex_array.lock();
      auto program = rendering_->program.lock();

      if (!vertex_array || vertex_array->vertex_array_id() == 0 || !program ||
          program->program_id() == 0)
        break;

      glBindVertexArray(vertex_array->vertex_array_id());
      glUseProgram(program->program_id());

      ApplyUniformVariables(program->program_id(), camera, model);

      ApplyGlTexture();

      auto args = rendering_->draw_args.arrays;
      glDrawArrays(args.mode, args.first, args.count);

      UnapplyGlTextures();
      glUseProgram(0);
      glBindVertexArray(0);
      break;
    }
  }
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

}  // namespace zen::remote::client
