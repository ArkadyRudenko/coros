#pragma once

#pragma once

#include <atomic>
#include <coroutine>
#include <iostream>
#include <mutex>  // std::unique_lock

#include <coros/support/forward_list.hpp>
#include <coros/support/mpsc_lock_free_stack.hpp>

namespace coros::tasks {

class Mutex {
  using UniqueLock = std::unique_lock<Mutex>;

  struct [[nodiscard]] Locker
      : public support::IntrusiveForwardListNode<Locker> {
    using CoroutineHandle = std::coroutine_handle<>;

    Mutex& mutex_;

    explicit Locker(Mutex& mutex) : mutex_(mutex) {}

    // Awaiter protocol

    // NOLINTNEXTLINE
    bool await_ready() { return mutex_.TryLock(); }

    // NOLINTNEXTLINE
    bool await_suspend(CoroutineHandle awaiting_coroutine) {
      coro_handle_ = awaiting_coroutine;
      return !mutex_.TryLockOrEnqueue(this);
    }

    // NOLINTNEXTLINE
    UniqueLock await_resume() { return UniqueLock(mutex_); }

    CoroutineHandle coro_handle_;
  };

 public:
  // Asynchronous
  auto ScopedLock() { return Locker{*this}; }

  bool TryLock() { return awaiters_.TryPushNothing(); }

  void lock() {}

  // For std::unique_lock
  // Do not use directly
  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  // Returns true if lock acquired
  bool TryLockOrEnqueue(Locker* locker) {
    while (true) {
      if (awaiters_.PushIfNotEmpty(locker)) {
        return false;
      }
      if (TryLock()) {
        return true;
      }
    }
  }

  void Unlock() {
    if (!awaiters_.TryPopNothing()) {
      auto locker = awaiters_.TryPop();  // TODO simply
      if (locker != nullptr) {
        locker->coro_handle_.resume();
      }
    }
  }

 private:
  support::MPSCStack<Locker> awaiters_;
};

}  // namespace coros::tasks