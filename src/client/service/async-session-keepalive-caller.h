#pragma once

#include "client/service/async-service-caller.h"
#include "core/logger.h"
#include "session.grpc.pb.h"

namespace zen::remote::client::service {

class AsyncSessionKeepaliveCaller : public IAsyncServiceCaller {
  enum State {
    kCreate,
    kProcess,
    kFinish,
  };

 public:
  static void Listen(SessionService::AsyncService* async_service,
      grpc::ServerCompletionQueue* completion_queue, Remote* remote)
  {
    new AsyncSessionKeepaliveCaller(async_service, completion_queue, remote);
  }

  void Finish()
  {
    state_ = kFinish;
    responder_.Finish(response_, grpc::Status::OK, this);
  }

 private:
  void Proceed()
  {
    switch (state_) {
      case kCreate:
        state_ = kProcess;
        async_service_->RequestKeepalive(&context_, &request_, &responder_,
            completion_queue_, completion_queue_, this);
        return;

      case kProcess: {
        new AsyncSessionKeepaliveCaller(
            async_service_, completion_queue_, remote_);
        auto session = remote_->current();
        if (!session || session->id() != request_.id() ||
            session->SetKeepaliveCaller(this) == false) {
          state_ = kFinish;
          responder_.Finish(response_,
              grpc::Status(grpc::ABORTED,
                  "Specified session is invalid or already attached a "
                  "keepalive request"),
              this);
        }
        return;
      }

      case kFinish:
        delete this;
        return;

      default:
        assert(false && "Unexpected state");
        return;
    }
  }

  void Cancel() { delete this; }

  AsyncSessionKeepaliveCaller(SessionService::AsyncService* async_service,
      grpc::ServerCompletionQueue* completion_queue, Remote* remote)
      : async_service_(async_service),
        remote_(remote),
        completion_queue_(completion_queue),
        responder_(&context_),
        state_(kCreate)
  {
    Proceed();
  }

  SessionService::AsyncService* async_service_;
  Remote* remote_;
  grpc::ServerCompletionQueue* completion_queue_;
  grpc::ServerContext context_;
  SessionKeepaliveRequest request_;
  SessionTerminateResponse response_;
  grpc::ServerAsyncResponseWriter<SessionTerminateResponse> responder_;
  State state_;
};

}  // namespace zen::remote::client::service
