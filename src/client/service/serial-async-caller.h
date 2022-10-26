#pragma once

#include "client/serial-command-queue.h"
#include "core/common.h"
#include "core/logger.h"

namespace zen::remote::client::service {

struct ISerialAsyncCaller {
  virtual ~ISerialAsyncCaller() = default;
  virtual void Proceed() = 0;
};

template <auto AsyncServiceRequest, auto Handler>
class SerialAsyncCaller final : public ISerialAsyncCaller {
  enum SerialAsyncCallerState {
    kCreate = 0,
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
      grpc::ServerCompletionQueue *completion_queue,
      SerialCommandQueue *command_queue)
  {
    new SerialAsyncCaller<AsyncServiceRequest, Handler>(
        async_service, service_impl, completion_queue, command_queue);
  }

  void Proceed() override
  {
    if (state_ == kCreate) {
      state_ = kProcess;
      std::invoke(AsyncServiceRequest, async_service_, &context_, &request_,
          &responder_, completion_queue_, completion_queue_, this);
    } else if (state_ == kProcess) {
      new SerialAsyncCaller<AsyncServiceRequest, Handler>(
          async_service_, service_impl_, completion_queue_, command_queue_);

      uint64_t serial = 0;

      auto result = context_.client_metadata().find(kGrpcMetadataSerialKey);
      if (result != context_.client_metadata().end()) {
        std::string serial_str(
            (*result).second.begin(), (*result).second.end());

        try {
          serial = std::stoull(serial_str);
        } catch (std::invalid_argument & /**/) {
          LOG_WARN("Invalid serial detected: invalid argument");
        } catch (std::out_of_range & /**/) {
          LOG_WARN("Invalid serial detected: out of range");
        }
      }

      auto command = CreateCommand([this] {
        auto grpc_status = std::invoke(
            Handler, service_impl_, &context_, &request_, &response_);

        state_ = kFinish;

        responder_.Finish(response_, grpc_status, this);
      });

      command_queue_->Push(serial, std::move(command));

    } else {
      delete this;
    }
  }

 private:
  SerialAsyncCaller(AsyncService *async_service, ServiceImpl *service_impl,
      grpc::ServerCompletionQueue *completion_queue,
      SerialCommandQueue *command_queue)
      : async_service_(async_service),
        service_impl_(service_impl),
        command_queue_(command_queue),
        completion_queue_(completion_queue),
        responder_(&context_),
        state_(kCreate)
  {
    Proceed();
  }

  AsyncService *async_service_;
  ServiceImpl *service_impl_;
  SerialCommandQueue *command_queue_;
  grpc::ServerCompletionQueue *completion_queue_;
  grpc::ServerContext context_;
  Request request_;
  Response response_;
  grpc::ServerAsyncResponseWriter<Response> responder_;
  SerialAsyncCallerState state_;
};

}  // namespace zen::remote::client::service
