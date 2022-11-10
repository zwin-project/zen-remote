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
  class Connection {
   public:
    Connection(typename std::list<std::unique_ptr<ICaller>>::iterator iter,
        Signal<T> *signal)
        : iter_(iter), signal_(signal)
    {
    }

    void Disconnect() { signal_->list_.erase(iter_); }

   private:
    typename std::list<std::unique_ptr<ICaller>>::iterator iter_;
    Signal<T> *signal_;
  };

  Signal(const Signal &) = delete;
  Signal(Signal &&) = delete;
  Signal &operator=(const Signal &) = delete;
  Signal &operator=(Signal &&) = delete;
  Signal() = default;

  template <typename F>
  std::unique_ptr<Connection> Connect(F &&func)
  {
    list_.emplace_back(
        std::unique_ptr<ICaller>(new Caller<F>(std::forward<F>(func))));
    auto iter = std::prev(list_.end());
    return std::make_unique<Connection>(iter, this);
  }

  void operator()(Args... args) const
  {
    // `iter` may be erased from the list in `Call`.
    auto iter = list_.begin();
    while (iter != list_.end()) {
      auto next = std::next(iter);
      (*iter)->Call(args...);
      iter = next;
    }
  }

 private:
  std::list<std::unique_ptr<ICaller>> list_;
};

}  // namespace zen::remote
