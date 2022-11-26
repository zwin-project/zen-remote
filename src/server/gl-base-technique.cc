#include "server/gl-base-technique.h"

#include "core/logger.h"
#include "gl-base-technique.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/job-queue.h"
#include "server/job.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

GlBaseTechnique::GlBaseTechnique(std::shared_ptr<Session> session)
    : id_(session->NewSerial(Session::kResource)), session_(std::move(session))
{
}

void
GlBaseTechnique::Init(uint64_t rendering_unit_id)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           rendering_unit_id](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBaseTechniqueService::Stub::PrepareAsyncNew>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::New");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);
    caller->request()->set_rendering_unit_id(rendering_unit_id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::BindProgram(uint64_t program_id)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           program_id](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlBaseTechniqueService::Stub::PrepareAsyncBindProgram>(std::move(stub),
        std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBaseTechnique::BindProgram");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_program_id(program_id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::BindVertexArray(uint64_t vertex_array_id)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           vertex_array_id](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlBaseTechniqueService::Stub::PrepareAsyncBindVertexArray>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBaseTechnique::BindVertexArray");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_vertex_array_id(vertex_array_id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::BindTexture(
    uint32_t binding, std::string name, uint64_t texture_id, uint32_t target)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job =
      CreateJob([id = id_, connection = session->connection(), context_raw,
                    grpc_queue = session->grpc_queue(), binding,
                    name = std::move(name), texture_id, target](bool cancel) {
        auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
        if (cancel) {
          return;
        }

        auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

        auto caller = new AsyncGrpcCaller<
            &GlBaseTechniqueService::Stub::PrepareAsyncBindTexture>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::BindTexture");
                connection->NotifyDisconnection();
              }
            });

        caller->request()->set_id(id);
        caller->request()->set_binding(binding);
        caller->request()->set_name(std::move(name));
        caller->request()->set_texture_id(texture_id);
        caller->request()->set_target(target);

        grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
      });

  session->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::GlUniformVector(uint32_t location, std::string name,
    uint32_t size, uint32_t count, int32_t* value)
{
  GlUniform(location, std::move(name), UNIFORM_VARIABLE_TYPE_INT, 1, size,
      count, false, value);
}

void
GlBaseTechnique::GlUniformVector(uint32_t location, std::string name,
    uint32_t size, uint32_t count, uint32_t* value)
{
  GlUniform(location, std::move(name), UNIFORM_VARIABLE_TYPE_UINT, 1, size,
      count, false, value);
}

void
GlBaseTechnique::GlUniformVector(uint32_t location, std::string name,
    uint32_t size, uint32_t count, float* value)
{
  GlUniform(location, std::move(name), UNIFORM_VARIABLE_TYPE_FLOAT, 1, size,
      count, false, value);
}

void
GlBaseTechnique::GlUniformMatrix(uint32_t location, std::string name,
    uint32_t col, uint32_t row, uint32_t count, bool transpose, float* value)
{
  GlUniform(location, std::move(name), UNIFORM_VARIABLE_TYPE_FLOAT, col, row,
      count, transpose, value);
}

void
GlBaseTechnique::GlUniform(uint32_t location, std::string name,
    UniformVariableType type, uint32_t col, uint32_t row, uint32_t count,
    bool transpose, void* value)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  size_t value_size = 4 * col * row * count;
  std::string value_copy(value_size, ' ');
  std::memcpy(value_copy.data(), value, value_size);

  auto job =
      CreateJob([id = id_, connection = session->connection(), context_raw,
                    grpc_queue = session->grpc_queue(), location,
                    name = std::move(name), type, col, row, count, transpose,
                    value = std::move(value_copy)](bool cancel) {
        auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
        if (cancel) {
          return;
        }

        auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

        auto caller = new AsyncGrpcCaller<
            &GlBaseTechniqueService::Stub::PrepareAsyncGlUniform>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::GlUniform");
                connection->NotifyDisconnection();
              }
            });

        caller->request()->set_id(id);
        caller->request()->set_location(location);
        caller->request()->set_name(std::move(name));
        caller->request()->set_type(type);
        caller->request()->set_col(col);
        caller->request()->set_row(row);
        caller->request()->set_count(count);
        caller->request()->set_transpose(transpose);
        caller->request()->set_value(std::move(value));

        grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
      });

  session->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::GlDrawArrays(uint32_t mode, int32_t first, uint32_t count)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob(
      [id = id_, connection = session->connection(), context_raw,
          grpc_queue = session->grpc_queue(), mode, first, count](bool cancel) {
        auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
        if (cancel) {
          return;
        }

        auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

        auto caller = new AsyncGrpcCaller<
            &GlBaseTechniqueService::Stub::PrepareAsyncGlDrawArrays>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::GlDrawArrays");
                connection->NotifyDisconnection();
              }
            });

        caller->request()->set_id(id);
        caller->request()->set_mode(mode);
        caller->request()->set_first(first);
        caller->request()->set_count(count);

        grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
      });

  session->job_queue()->Push(std::move(job));
}

void
GlBaseTechnique::GlDrawElements(uint32_t mode, uint32_t count, uint32_t type,
    uint64_t offset, uint64_t element_array_buffer_id)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           mode, count, type, offset,
                           element_array_buffer_id](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlBaseTechniqueService::Stub::PrepareAsyncGlDrawElements>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlBaseTechnique::GlDrawElements");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_mode(mode);
    caller->request()->set_count(count);
    caller->request()->set_type(type);
    caller->request()->set_offset(offset);
    caller->request()->set_element_array_buffer_id(element_array_buffer_id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

GlBaseTechnique::~GlBaseTechnique()
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw,
                           grpc_queue = session->grpc_queue()](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlBaseTechniqueService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlBaseTechniqueService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlBaseTechnique::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

uint64_t
GlBaseTechnique::id()
{
  return id_;
}

std::unique_ptr<IGlBaseTechnique>
CreateGlBaseTechnique(
    std::shared_ptr<ISession> session, uint64_t rendering_unit_id)
{
  auto gl_base_technique = std::make_unique<GlBaseTechnique>(
      std::dynamic_pointer_cast<Session>(session));

  gl_base_technique->Init(rendering_unit_id);

  return gl_base_technique;
}

}  // namespace zen::remote::server
