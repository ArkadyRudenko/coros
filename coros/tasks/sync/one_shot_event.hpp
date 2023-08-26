#pragma once

#include <atomic>
#include <coroutine>
#include <mutex>

#include <coros/support/forward_list.hpp>
#include <coros/support/mpsc_lock_free_stack.hpp>
#include <coros/support/spinlock.hpp>

namespace coros::tasks {

class OneShotEvent {
 public:
  struct WaitGroupAwaiter
      : public support::IntrusiveForwardListNode<WaitGroupAwaiter> {
    using CoroutineHandle = std::coroutine_handle<>;

    OneShotEvent& one_shot_event_;
    CoroutineHandle coro_handle_;

    explicit WaitGroupAwaiter(OneShotEvent& ev) : one_shot_event_(ev) {}

    // Awaiter protocol

    // NOLINTNEXTLINE
    bool await_ready() { return one_shot_event_.awaiters_.IsEmpty(); }

    // NOLINTNEXTLINE
    bool await_suspend(CoroutineHandle awaiting_coroutine) {
      if (await_ready()) {
        return false;
      }
      coro_handle_ = awaiting_coroutine;
      return one_shot_event_.awaiters_.PushIfNotEmpty(this);
    }

    // NOLINTNEXTLINE
    void await_resume() {}
  };

  OneShotEvent() = default;

  // Asynchronous
  auto Wait() { return WaitGroupAwaiter{*this}; }

  // One-shot
  // We can not lock spinlock in ~WaitGroup -> deadlock, because Fire(){ lock() -> handle_.resume() } -> ~WaitGroup()
  void Fire() {
    WaitGroupAwaiter* h;
    WaitGroupAwaiter* prev = nullptr;
    while (true) {
      h = awaiters_.TryPop();
      if (h == nullptr && awaiters_.TryPopNothing()) {
        break;
      }
      if (h != nullptr) {
        if (prev == nullptr) {
          prev = h;
          continue;
        }
        h->coro_handle_.resume();
      }
    }
    if (prev != nullptr) {
      prev->coro_handle_.resume();
    }
  }

  void Lock() {
    awaiters_.TryPushNothing();
  }

 private:
  support::MPSCStack<WaitGroupAwaiter> awaiters_;
};

}  // namespace coros::tasks