#pragma once

#include <mutex>
#include <queue>
#include <thread>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/executor.hpp>
#include <coros/support/spinlock.hpp>
#include <coros/timers/timer.hpp>

namespace coros::timer {

class TimerScheduler {

  using Clock = std::chrono::steady_clock;
  using TimePoint = Clock::time_point;

  struct Timer {
    TimePoint deadline_;
    TimerBase* timer_;

    bool operator<(const Timer& rhs) const { return deadline_ > rhs.deadline_; }
  };

 public:
  TimerScheduler() : worker_thread_([this]() { Work(); }) {}

  void AddTimer(TimerBase* timer) {
    std::unique_lock lock(spinlock_);
    if (stop_request_) {
      return;
    }
    timer->GetExecutor().Execute(nullptr, executors::Hint::AddTimer);
    timers_.push(Timer{Clock::now() + timer->Delay(), timer});
  }

  void Stop() {
    stop_request_.store(true);
    worker_thread_.join();
  }

  ~TimerScheduler() {
    if (!stop_request_) {
      Stop();
    }
  }

 private:
  void Work() {
    while (!stop_request_.load()) { // std::memory_order_relaxed
      auto* timers = GetReadyTimers();
      while (timers != nullptr) {
        auto* next = (TimerBase*)timers->Next();
        timers->Alarm();
        timers->GetExecutor().Execute(nullptr, executors::Hint::RemoveTimer);
        timers = next;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // TODO: if there are some timers - Alarm All?
  }

  TimerBase* GetReadyTimers() {
    const auto now = Clock::now();
    TimerBase* ready_timers = nullptr;
    std::unique_lock lock(spinlock_);
    while (!timers_.empty()) {
      const auto& next = timers_.top();

      if (next.deadline_ > now) {
        break;
      }

      {
        next.timer_->SetNext(ready_timers);
        ready_timers = next.timer_;
      }

      timers_.pop();
    }
    return ready_timers;
  }

 private:
  std::priority_queue<Timer> timers_;
  support::SpinLock spinlock_;
  std::atomic<bool> stop_request_{false};
  std::thread worker_thread_;
};

}  // namespace coros::timer