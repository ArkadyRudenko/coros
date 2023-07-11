#pragma once

#include <coroutine>
#include <exception>
#include <optional>
#include <utility>

#include <coros/support/unit.hpp>

namespace coros::tasks {

template <typename T = support::Unit>
struct Task {

  using ValueType = T;

  template <typename PromiseType = T>
  struct Promise {
    // NOLINTNEXTLINE
    auto get_return_object() {
      return Task{std::coroutine_handle<Promise>::from_promise(*this)};
    }

    // NOLINTNEXTLINE
    auto initial_suspend() noexcept { return std::suspend_always{}; }

    // NOLINTNEXTLINE
    auto final_suspend() noexcept { return std::suspend_never{}; }

    // NOLINTNEXTLINE
    void set_exception(std::exception_ptr) {
      std::terminate();  // Not implemented
    }

    // NOLINTNEXTLINE
    void unhandled_exception() {
      std::terminate();  // Not implemented
    }

    void return_value(std::optional<T>&& res) { result = std::move(res); }

    std::optional<T> result;  // TODO
  };

  template <>
  struct Promise<support::Unit> {
    // NOLINTNEXTLINE
    auto get_return_object() {
      return Task{std::coroutine_handle<Promise>::from_promise(*this)};
    }

    // NOLINTNEXTLINE
    auto initial_suspend() noexcept { return std::suspend_always{}; }

    // NOLINTNEXTLINE
    auto final_suspend() noexcept { return std::suspend_never{}; }

    // NOLINTNEXTLINE
    void set_exception(std::exception_ptr) {
      std::terminate();  // Not implemented
    }

    // NOLINTNEXTLINE
    void unhandled_exception() {
      std::terminate();  // Not implemented
    }

    // NOLINTNEXTLINE
    void return_void() {
      // Not implemented
    }
  };

  using CoroutineHandle = std::coroutine_handle<Promise<T>>;

  explicit Task(CoroutineHandle callee) : callee_(callee) {}

  Task(Task&&) = default;

  // Non-copyable
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  ~Task() {
    if (callee_ && !callee_.done()) {
      std::terminate();
    }
  }

  CoroutineHandle ReleaseCoroutine() {
    return std::exchange(callee_, CoroutineHandle());
  }

 private:
  CoroutineHandle callee_;
};

}  // namespace coros::tasks

//////////////////////////////////////////////////////////////////////

template <typename T, typename... Args>
struct std::coroutine_traits<coros::tasks::Task<T>, Args...> {
  using promise_type =
      typename coros::tasks::Task<T>::template Promise<T>;  // NOLINT
};
