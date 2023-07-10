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

    explicit WaitGroupAwaiter(OneShotEvent& wait_group)
        : one_shot_event_(wait_group) {}

    // Awaiter protocol

    // NOLINTNEXTLINE
    bool await_ready() {
      std::lock_guard lock(one_shot_event_.spinlock_);
      return one_shot_event_.wait_count_.load() == 0;
    }

    // NOLINTNEXTLINE
    bool await_suspend(CoroutineHandle awaiting_coroutine) {
      coro_handle_ = awaiting_coroutine;
      std::lock_guard lock(one_shot_event_.spinlock_);
      if (one_shot_event_.wait_count_.load() == 0) {
        return false;
      }
      one_shot_event_.Push(this);
      return true;
    }

    // NOLINTNEXTLINE
    void await_resume() {}

    CoroutineHandle coro_handle_;
  };

  explicit OneShotEvent(std::atomic<size_t>& wait_count,
                        exe::support::SpinLock& spinlock)
      : wait_count_(wait_count), spinlock_(spinlock) {}

  // Asynchronous
  auto Wait() { return WaitGroupAwaiter{*this}; }

  // One-shot
  // We can not lock spinlock in ~WaitGroup -> deadlock, because Fire(){ lock() -> handle_.resume() } -> ~WaitGroup()
  void Fire(std::unique_lock<exe::support::SpinLock>& lock) {
    while (true) {
      auto awaiter = awaiters_.TryPop();
      if (awaiter == nullptr) {
        lock.unlock();
        return;
      }
      bool exit = awaiter->next_ == nullptr;
      if (exit) {
        lock.unlock();
      }
      awaiter->coro_handle_.resume();
      if (exit) {
        return;
      }
    }
  }

 private:
  void Push(WaitGroupAwaiter* wait_group_awaiter) {
    awaiters_.PushIsFirst(wait_group_awaiter);
  }

 private:
  std::atomic<size_t>& wait_count_;
  exe::support::SpinLock& spinlock_;
  support::MPSCStack<WaitGroupAwaiter> awaiters_;
};

}  // namespace coros::tasks