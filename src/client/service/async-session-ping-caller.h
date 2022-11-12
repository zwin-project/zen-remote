#pragma once

#include "client/remote.h"
#include "client/service/async-service.h"
#include "session.grpc.pb.h"

namespace zen::remote::client::service {

class AsyncSessionPingCaller {
  enum State {
    kCreate = 0,
    kConnect,
    kRead,
    kFinish,
  };

  struct Tag final : public IAsyncServiceCaller {
    State type;
    AsyncSessionPingCaller* caller;

    Tag(State type, AsyncSessionPingCaller* caller) : type(type), caller(caller)
    {
    }

    void Proceed() override { caller->Proceed(type); }

    void Cancel() override { caller->Cancel(); }
  };

 public:
  static void Listen(SessionService::AsyncService* async_service,
      grpc::ServerCompletionQueue* completion_queue, Remote* remote)
  {
    new AsyncSessionPingCaller(async_service, completion_queue, remote);
  }

 private:
  void Proceed(State state)
  {
    switch (state) {
      case kCreate:
        async_service_->RequestPing(&context_, &reader_, completion_queue_,
            completion_queue_, &connect_tag_);
        return;

      case kConnect:
        new AsyncSessionPingCaller(async_service_, completion_queue_, remote_);
        reader_.Read(&request_, &read_tag_);
        return;

      case kRead:
        session_id_ = request_.id();
        if (request_.done()) {
          SessionTerminateResponse response;
          reader_.Finish(response, grpc::Status::OK, &finish_tag_);
        } else {
          reader_.Read(&request_, &read_tag_);
        }
        return;

      case kFinish:
        delete this;
        return;

      default:
        assert(false && "Unexpected state");
        return;
    }
  }

  void Cancel() { delete this; }

  AsyncSessionPingCaller(SessionService::AsyncService* async_service,
      grpc::ServerCompletionQueue* completion_queue, Remote* remote)
      : async_service_(async_service),
        remote_(remote),
        completion_queue_(completion_queue),
        reader_(&context_),
        connect_tag_(kConnect, this),
        read_tag_(kRead, this),
        finish_tag_(kFinish, this)
  {
    Proceed(kCreate);
  }

  ~AsyncSessionPingCaller()
  {
    if (remote_->session_manager()->current() &&
        session_id_ == remote_->session_manager()->current()->id()) {
      remote_->session_manager()->ClearCurrent();
    }
  }

  SessionPingRequest request_;

  SessionService::AsyncService* async_service_;
  Remote* remote_;
  grpc::ServerCompletionQueue* completion_queue_;
  grpc::ServerContext context_;
  grpc::ServerAsyncReader<SessionTerminateResponse, SessionPingRequest> reader_;
  Tag connect_tag_;
  Tag read_tag_;
  Tag finish_tag_;
  uint64_t session_id_ = 0;
};

}  // namespace zen::remote::client::service
