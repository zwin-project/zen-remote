#include "server/gl-sampler.h"

#include <GLES3/gl32.h>

#include "core/logger.h"
#include "gl-sampler.grpc.pb.h"
#include "server/async-grpc-caller.h"
#include "server/async-grpc-queue.h"
#include "server/serial-request-context.h"
#include "server/session.h"

namespace zen::remote::server {

namespace {

size_t
CalcParameterSize(uint32_t pname)
{
  switch (pname) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
      return 4;

    case GL_TEXTURE_BORDER_COLOR:
      return 16;

    default:
      return 0;
  }
}

}  // namespace

GlSampler::GlSampler(std::shared_ptr<Session> session)
    : id_(session->NewSerial(Session::kResource)), session_(std::move(session))
{
}

void
GlSampler::Init()
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

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<&GlSamplerService::Stub::PrepareAsyncNew>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlSampler::New");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

void
GlSampler::GlSamplerParameterf(uint32_t pname, float param)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           pname, param](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlSamplerService::Stub::PrepareAsyncGlSamplerParameterf>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlSampler::GlSamplerParameterf");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_pname(pname);
    caller->request()->set_param(param);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
};

void
GlSampler::GlSamplerParameteri(uint32_t pname, int32_t param)
{
  auto session = session_.lock();
  if (!session) return;

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           pname, param](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlSamplerService::Stub::PrepareAsyncGlSamplerParameteri>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlSampler::GlSamplerParameteri");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_pname(pname);
    caller->request()->set_param(param);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
};

void
GlSampler::GlSamplerParameterfv(uint32_t pname, const float* params)
{
  auto session = session_.lock();
  if (!session) return;

  size_t params_size = CalcParameterSize(pname);
  std::string params_copy(params_size, ' ');
  std::memcpy(params_copy.data(), params, params_size);

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           pname,
                           params = std::move(params_copy)](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlSamplerService::Stub::PrepareAsyncGlSamplerParameterfv>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlSampler::GlSamplerParameterfv");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_pname(pname);
    caller->request()->set_params(std::move(params));

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
};

void
GlSampler::GlSamplerParameteriv(uint32_t pname, const int32_t* params)
{
  auto session = session_.lock();
  if (!session) return;

  size_t params_size = CalcParameterSize(pname);
  std::string params_copy(params_size, ' ');
  std::memcpy(params_copy.data(), params, params_size);

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           pname,
                           params = std::move(params_copy)](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlSamplerService::Stub::PrepareAsyncGlSamplerParameteriv>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlSampler::GlSamplerParameteriv");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_pname(pname);
    caller->request()->set_params(std::move(params));

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
};

void
GlSampler::GlSamplerParameterIiv(uint32_t pname, const int32_t* params)
{
  auto session = session_.lock();
  if (!session) return;

  size_t params_size = CalcParameterSize(pname);
  std::string params_copy(params_size, ' ');
  std::memcpy(params_copy.data(), params, params_size);

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           pname,
                           params = std::move(params_copy)](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlSamplerService::Stub::PrepareAsyncGlSamplerParameterIiv>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlSampler::GlSamplerParameterIiv");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_pname(pname);
    caller->request()->set_params(std::move(params));

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
};

void
GlSampler::GlSamplerParameterIuiv(uint32_t pname, const uint32_t* params)
{
  auto session = session_.lock();
  if (!session) return;

  size_t params_size = CalcParameterSize(pname);
  std::string params_copy(params_size, ' ');
  std::memcpy(params_copy.data(), params, params_size);

  auto context_raw = new SerialRequestContext(session.get());

  auto job = CreateJob([id = id_, connection = session->connection(),
                           context_raw, grpc_queue = session->grpc_queue(),
                           pname,
                           params = std::move(params_copy)](bool cancel) {
    auto context = std::unique_ptr<grpc::ClientContext>(context_raw);
    if (cancel) {
      return;
    }

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller = new AsyncGrpcCaller<
        &GlSamplerService::Stub::PrepareAsyncGlSamplerParameterIuiv>(
        std::move(stub), std::move(context),
        [connection](EmptyResponse* /*response*/, grpc::Status* status) {
          if (!status->ok() && status->error_code() != grpc::CANCELLED) {
            LOG_WARN("Failed to call remote GlSampler::GlSamplerParameterIuiv");
            connection->NotifyDisconnection();
          }
        });

    caller->request()->set_id(id);
    caller->request()->set_pname(pname);
    caller->request()->set_params(std::move(params));

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
};

GlSampler::~GlSampler()
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

    auto stub = GlSamplerService::NewStub(connection->grpc_channel());

    auto caller =
        new AsyncGrpcCaller<&GlSamplerService::Stub::PrepareAsyncDelete>(
            std::move(stub), std::move(context),
            [connection](EmptyResponse* /*response*/, grpc::Status* status) {
              if (!status->ok() && status->error_code() != grpc::CANCELLED) {
                LOG_WARN("Failed to call remote GlSampler::Delete");
                connection->NotifyDisconnection();
              }
            });

    caller->request()->set_id(id);

    grpc_queue->Push(std::unique_ptr<AsyncGrpcCallerBase>(caller));
  });

  session->job_queue()->Push(std::move(job));
}

uint64_t
GlSampler::id()
{
  return id_;
}

std::unique_ptr<IGlSampler>
CreateGlSampler(std::shared_ptr<ISession> session)
{
  auto gl_sampler =
      std::make_unique<GlSampler>(std::dynamic_pointer_cast<Session>(session));
  gl_sampler->Init();

  return gl_sampler;
}

}  // namespace zen::remote::server
