#pragma once

#include <list>
#include <memory>

namespace zen::remote {

template <typename T>
class Signal {
};

template <typename R, class... Args>
class Signal<R(Args...)> {
  using T = R(Args...);

  struct ICaller {
    virtual ~ICaller() = default;
    virtual void Call(Args... args) = 0;
  };

  template <typename F>
  class Caller final : public ICaller {
   public:
    Caller(F &&func) : func_(std::forward<F>(func)) {}
    void Call(Args... args) override { func_(args...); }

   private:
    F func_;
  };

 public:
  Signal(const Signal &) = delete;
  Signal(Signal &&) = delete;
  Signal &operator=(const Signal &) = delete;
  Signal &operator=(Signal &&) = delete;
  Signal() = default;

  template <typename F>
  void Connect(F &&func)
  {
    list_.emplace_back(
        std::unique_ptr<ICaller>(new Caller<F>(std::forward<F>(func))));
  }

  void operator()(Args... args) const
  {
    for (auto &callback : list_) {
      callback->Call(args...);
    }
  }

 public:
  std::list<std::unique_ptr<ICaller>> list_;
};

}  // namespace zen::remote
