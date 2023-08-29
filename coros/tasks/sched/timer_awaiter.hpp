#pragma once

#include <chrono>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/tasks/awaiter.hpp>
#include <coros/timers/timer.hpp>
#include <coros/timers/timers_scheduler.hpp>

namespace coros::tasks {

template <typename Rep, typename Period>
class TimerAwaiter final : public timer::TimerBase, public TaskBase, Awaiter<> {

  using Pool = executors::compute::ThreadPool;

  using Duration = std::chrono::duration<Rep, Period>;

 public:
  explicit TimerAwaiter(Duration duration, Pool& pool)
      : duration_(duration), pool_(pool) {}

  bool await_ready() { return false; }

  void await_suspend(CoroutineHandle caller) {
    SetCoroutine(caller);
    pool_.timer_->AddTimer(this);
  }

  void await_resume() {}

  [[nodiscard]] timer::Millis Delay() const override {
    return timer::ToMillis(duration_);
  }

  void Alarm() override { pool_.Execute(this); }

  void Run() noexcept override { Resume(); }

  void Discard() noexcept override { Destroy(); }

  [[nodiscard]] executors::IExecutor& GetExecutor() const override {
    return pool_;
  }

  ~TimerAwaiter() override { Destroy(); }

 private:
  std::chrono::system_clock::duration duration_;
  Pool& pool_;
};

template <typename Rep, typename Period>
std::chrono::duration<Rep, Period> SleepFor(
    std::chrono::duration<Rep, Period> duration) noexcept {
  return duration;
}

}  // namespace coros::tasks

template <typename Rep, typename Period>
auto operator co_await(std::chrono::duration<Rep, Period> duration) noexcept {
  return coros::tasks::TimerAwaiter{
      duration, *coros::executors::compute::ThreadPool::Current()};
}