#include "client/gl-base-technique.h"

#include "client/atomic-command-queue.h"
#include "client/gl-program.h"
#include "client/gl-vertex-array.h"
#include "client/tmp-rendering-helper.h"
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
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  pending_.draw_method = DrawMethod::kArrays;
  pending_.draw_args.arrays.mode = mode;
  pending_.draw_args.arrays.count = count;
  pending_.draw_args.arrays.first = first;
  pending_.draw_method_damaged = true;
}

void
GlBaseTechnique::Render(Camera* camera)
{
  switch (rendering_->draw_method) {
    case DrawMethod::kNone: {  // FIXME:
      static auto program_id = TmpRenderingHelper::CompilePrograms(
          default_vertex_shader, default_color_fragment_shader);
      auto vertex_array = rendering_->vertex_array.lock();

      if (!vertex_array) break;

      glBindVertexArray(vertex_array->vertex_array_id());
      glUseProgram(program_id);

      auto mvp_location = glGetUniformLocation(program_id, "mvp");
      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (float*)&camera->vp);

      glDrawArrays(GL_LINES, 0, 8);

      glUseProgram(0);
      glBindVertexArray(0);

      break;
    }

    case DrawMethod::kArrays: {
      auto args = rendering_->draw_args.arrays;
      glDrawArrays(args.mode, args.first, args.count);
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
