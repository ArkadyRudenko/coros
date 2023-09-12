#pragma once

#include <atomic>
#include <concepts>
#include <coroutine>
#include <exception>
#include <optional>
#include <type_traits>
#include <utility>

#include <coros/executors/executor.hpp>
#include <coros/executors/inline/inline_executor.hpp>
#include <coros/support/result.hpp>
#include <coros/support/unit.hpp>

namespace coros::tasks {

template <typename T = support::Unit>
struct Task {

  struct Promise {

    using Res = support::Result<T>;

    struct FinalAwaitable {
      bool await_ready() const noexcept { return false; }

      void await_resume() noexcept {}

      template <typename Promise>
      void await_suspend(std::coroutine_handle<Promise> h) noexcept {
        auto continuation = h.promise().caller_;
        if (continuation) {
          continuation.resume();
        }
      }
    };

    // NOLINTNEXTLINE
    auto get_return_object() {
      return Task{std::coroutine_handle<Promise>::from_promise(*this)};
    }

    // NOLINTNEXTLINE
    std::suspend_always initial_suspend() noexcept { return {}; }

    // NOLINTNEXTLINE
    FinalAwaitable final_suspend() noexcept { return FinalAwaitable{}; }

    // NOLINTNEXTLINE
    void set_exception(std::exception_ptr exception_ptr) {
      result_ = Res::Err(std::move(exception_ptr));
    }

    // NOLINTNEXTLINE
    void unhandled_exception() {
      result_ = Res::Err(std::move(std::current_exception()));
    }

    void return_value(T res) { result_ = Res::Ok(std::move(res)); }

    void SetCaller(std::coroutine_handle<> caller) { caller_ = caller; }

    Res GetResult() { return std::move(*result_); }

   private:
    std::optional<Res> result_;
    std::coroutine_handle<> caller_;
  };

  using CoroutineHandle = std::coroutine_handle<Promise>;

  explicit Task(CoroutineHandle callee) : callee_(callee) {
    if (executors::compute::ThreadPool::Current() != nullptr) {
      executor_ = executors::compute::ThreadPool::Current();
    }
  }

  Task(Task&& other)  noexcept : callee_(other.callee_) { other.callee_ = nullptr; }

  // Non-copyable
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  // TODO: error:
  ~Task() {
    if (callee_ && callee_.done()) {
      callee_.destroy();
    }
  }

  CoroutineHandle ReleaseCoroutine() {
    return std::exchange(callee_, CoroutineHandle());
  }

  CoroutineHandle GetCoroutine() { return callee_; }

  Task<T> Via(executors::IExecutor& executor) && {
    return std::move(Task<T>{ReleaseCoroutine(), executor});
  }

  executors::IExecutor& GetExecutor() { return *executor_; }

  template <std::invocable Fn>
  Task<T> Then(Fn&& /*func*/) && {  // TODO
    return std::move(*this);
  }

 private:
  explicit Task(CoroutineHandle callee, executors::IExecutor& executor)
      : callee_(callee), executor_(&executor) {}

 private:
  CoroutineHandle callee_;
  executors::IExecutor* executor_ = &executors::Inline();
};

}  // namespace coros::tasks

//////////////////////////////////////////////////////////////////////

template <typename T, typename... Args>
struct std::coroutine_traits<coros::tasks::Task<T>, Args...> {
  using promise_type = typename coros::tasks::Task<T>::Promise;  // NOLINT
};