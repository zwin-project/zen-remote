#pragma once

#include "client/remote.h"
#include "client/session.h"
#include "core/common.h"
#include "core/logger.h"

namespace zen::remote::client::service {

struct IAsyncServiceCaller {
  virtual ~IAsyncServiceCaller() = default;
  virtual void Proceed() = 0;
};

template <auto AsyncServiceRequest, auto Handler>
class AsyncServiceCaller final : public IAsyncServiceCaller {
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
    new AsyncServiceCaller<AsyncServiceRequest, Handler>(
        async_service, service_impl, completion_queue, remote);
  }

  void Proceed() override
  {
    if (state_ == kCreate) {
      state_ = kProcess;
      std::invoke(AsyncServiceRequest, async_service_, &context_, &request_,
          &responder_, completion_queue_, completion_queue_, this);
    } else if (state_ == kProcess) {
      new AsyncServiceCaller<AsyncServiceRequest, Handler>(
          async_service_, service_impl_, completion_queue_, remote_);

      auto grpc_status =
          std::invoke(Handler, service_impl_, &context_, &request_, &response_);

      state_ = kFinish;

      responder_.Finish(response_, grpc_status, this);
    } else {
      delete this;
    }
  }

 private:
  AsyncServiceCaller(AsyncService *async_service, ServiceImpl *service_impl,
      grpc::ServerCompletionQueue *completion_queue, Remote *remote)
      : async_service_(async_service),
        service_impl_(service_impl),
        remote_(remote),
        completion_queue_(completion_queue),
        responder_(&context_),
        state_(kCreate)
  {
    Proceed();
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
