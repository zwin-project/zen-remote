#pragma once

#include "client/remote.h"
#include "client/service/async-service-caller.h"
#include "client/session.h"
#include "core/common.h"
#include "core/logger.h"

namespace zen::remote::client::service {

/**
 * Handler implementation will be called only when the current session exists
 * and the request is for the session
 */
template <auto AsyncServiceRequest, auto Handler>
class AsyncSessionServiceCaller final : public IAsyncServiceCaller {
  enum State {
    kCreate,
    kProcess,
    kFinish,
  };

  using AsyncService = typename class_type<decltype(AsyncServiceRequest)>::type;

  using ServiceImpl = typename class_type<decltype(Handler)>::type;

  using RequestPtr = typename arg_type<1, decltype(Handler)>::type;
  using RequestConst = typename std::remove_pointer<RequestPtr>::type;
  using Request = typename std::remove_const<RequestConst>::type;

  using ResponsePtr = typename arg_type<2, decltype(Handler)>::type;
  using Response = typename std::remove_pointer<ResponsePtr>::type;

 public:
  static void Listen(AsyncService *async_service, ServiceImpl *service_impl,
      grpc::ServerCompletionQueue *completion_queue, Remote *remote)
  {
    new AsyncSessionServiceCaller<AsyncServiceRequest, Handler>(
        async_service, service_impl, completion_queue, remote);
  }

  void Proceed() override
  {
    if (state_ == kCreate) {
      state_ = kProcess;
      std::invoke(AsyncServiceRequest, async_service_, &context_, &request_,
          &responder_, completion_queue_, completion_queue_, this);
    } else if (state_ == kProcess) {
      new AsyncSessionServiceCaller<AsyncServiceRequest, Handler>(
          async_service_, service_impl_, completion_queue_, remote_);

      uint64_t session_id, request_serial;
      if (IsSessionContext(&session_id, &request_serial) == false) {
        state_ = kFinish;
        responder_.Finish(response_,
            grpc::Status(grpc::ABORTED, "Client context is invalid"), this);
        return;
      }

      auto session = remote_->current();
      if (!session || session->id() != session_id) {
        state_ = kFinish;
        responder_.Finish(
            response_, grpc::Status(grpc::ABORTED, "Session expired"), this);
        return;
      }

      auto command = CreateCommand([this](bool cancel) {
        if (cancel) {
          state_ = kFinish;
          responder_.Finish(
              response_, grpc::Status(grpc::ABORTED, "Session expired"), this);
          return;
        }

        // If the command is not cancelled, which means the session that has a
        // command queue for the command is still alive and current one.

        auto grpc_status = std::invoke(
            Handler, service_impl_, &context_, &request_, &response_);

        state_ = kFinish;

        responder_.Finish(response_, grpc_status, this);
      });

      if (!session->PushCommand(request_serial, std::move(command))) {
        state_ = kFinish;
        responder_.Finish(
            response_, grpc::Status(grpc::ABORTED, "Session expired"), this);
        return;
      }

    } else {
      delete this;
    }
  }

  void Cancel() override { delete this; }

 private:
  AsyncSessionServiceCaller(AsyncService *async_service,
      ServiceImpl *service_impl, grpc::ServerCompletionQueue *completion_queue,
      Remote *remote)
      : async_service_(async_service),
        service_impl_(service_impl),
        remote_(remote),
        completion_queue_(completion_queue),
        responder_(&context_),
        state_(kCreate)
  {
    Proceed();
  }

  bool StringRefToUint64(grpc::string_ref string_ref, uint64_t *n) const
  {
    std::string str(string_ref.begin(), string_ref.end());

    try {
      *n = std::stoull(str);
    } catch (std::invalid_argument & /**/) {
      LOG_WARN("Invalid serial detected: invalid argument");
      return false;
    } catch (std::out_of_range & /**/) {
      LOG_WARN("Invalid serial detected: out of range");
      return false;
    }

    return true;
  }

  bool IsSessionContext(uint64_t *session_id, uint64_t *request_serial) const
  {
    uint64_t session_id_tmp, request_serial_tmp;

    auto result = context_.client_metadata().find(kGrpcMetadataSessionKey);
    if (result == context_.client_metadata().end()) {
      return false;
    }

    if (StringRefToUint64((*result).second, &session_id_tmp) == false)
      return false;

    result = context_.client_metadata().find(kGrpcMetadataSerialKey);
    if (result == context_.client_metadata().end()) {
      return false;
    }

    if (StringRefToUint64((*result).second, &request_serial_tmp) == false)
      return false;

    *session_id = session_id_tmp;
    *request_serial = request_serial_tmp;

    return true;
  }

  AsyncService *async_service_;
  ServiceImpl *service_impl_;
  Remote *remote_;
  grpc::ServerCompletionQueue *completion_queue_;
  grpc::ServerContext context_;
  Request request_;
  Response response_;
  grpc::ServerAsyncResponseWriter<Response> responder_;
  State state_;
};

}  // namespace zen::remote::client::service
