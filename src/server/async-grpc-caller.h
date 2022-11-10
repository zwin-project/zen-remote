#pragma once

#include <type_traits>

#include "core/common.h"
#include "server/session-connection.h"

namespace zen::remote::server {

struct AsyncGrpcCallerBase {
  virtual ~AsyncGrpcCallerBase() = default;
  virtual void Start(grpc::CompletionQueue *cq) = 0;
  virtual void Finish() = 0;
};

template <auto PrepareAsync>
class AsyncGrpcCaller final : public AsyncGrpcCallerBase {
  template <typename T>
  struct template_parameter {
    using type = T;
  };

  template <template <typename> typename T, typename P>
  struct template_parameter<T<P>> {
    using type = P;
  };

  using RequestRef = typename arg_type<1, decltype(PrepareAsync)>::type;
  using RequestConst = typename std::remove_reference<RequestRef>::type;
  using Request = typename std::remove_const<RequestConst>::type;

  using Stub = typename class_type<decltype(PrepareAsync)>::type;

  using ResponseReader = typename std::invoke_result<decltype(PrepareAsync),
      Stub, typename arg_type<0, decltype(PrepareAsync)>::type,
      typename arg_type<1, decltype(PrepareAsync)>::type,
      typename arg_type<2, decltype(PrepareAsync)>::type>::type;

  using Response =
      typename template_parameter<typename ResponseReader::element_type>::type;

  using Callback = std::function<void(Response *, grpc::Status *status)>;

 public:
  DISABLE_MOVE_AND_COPY(AsyncGrpcCaller);
  AsyncGrpcCaller(std::unique_ptr<Stub> stub,
      std::unique_ptr<grpc::ClientContext> context, Callback &&callback)
      : context_(std::move(context)),
        stub_(std::move(stub)),
        callback_(std::forward<Callback>(callback))
  {
  }

  void Start(grpc::CompletionQueue *cq) override
  {
    response_reader_ =
        std::invoke(PrepareAsync, stub_.get(), context_.get(), request_, cq);

    response_reader_->StartCall();

    response_reader_->Finish(&response_, &status_, this);
  };

  void Finish() override { callback_(&response_, &status_); };

  inline Request *request() { return &request_; }

 private:
  Request request_;
  Response response_;
  grpc::Status status_;
  std::unique_ptr<grpc::ClientContext> context_;
  ResponseReader response_reader_;
  std::unique_ptr<Stub> stub_;
  Callback callback_;
};

}  // namespace zen::remote::server
