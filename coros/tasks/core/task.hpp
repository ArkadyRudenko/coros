#pragma once

#include <atomic>
#include <coroutine>
#include <exception>
#include <optional>
#include <utility>

#include <coros/executors/executor.hpp>
#include <coros/support/unit.hpp>

namespace coros::tasks {

std::atomic<int> a{0};

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

    void return_value(T res) {
      *res_ = std::move(res);
      if (caller_) {
        std::exchange(caller_, CoroutineHandle()).resume();
      }
    }

    void SetCaller(std::coroutine_handle<> caller) { caller_ = caller; }

    //    std::optional<T> GetResult() { return std::move(result_); }

    void SetResultPlace(T* res) { res_ = res; }

   private:
    T* res_;  // TODO: Result<T>
    std::coroutine_handle<> caller_;
  };

  template <>
  struct Promise<support::Unit> {
    // NOLINTNEXTLINE
    auto get_return_object() {
      return Task{
          std::coroutine_handle<Promise<support::Unit>>::from_promise(*this)};
    }

    // NOLINTNEXTLINE
    std::suspend_always initial_suspend() noexcept { return {}; }

    // NOLINTNEXTLINE
    std::suspend_never final_suspend() noexcept { return {}; }

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
      if (caller_) {
        std::exchange(caller_, CoroutineHandle()).resume();
      }
    }

    void SetCaller(std::coroutine_handle<> caller) { caller_ = caller; }

   private:
    std::coroutine_handle<> caller_;
  };

  using CoroutineHandle = std::coroutine_handle<Promise<T>>;

  explicit Task(CoroutineHandle callee)
      : callee_(callee), name_("name" + std::to_string(a.fetch_add(1))) {}

  Task(Task&&) = default;

  // Non-copyable
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

// TODO: error:
//  ~Task() {
//    if (callee_ != nullptr) {
//      callee_.destroy();
//    }
//  }

  CoroutineHandle ReleaseCoroutine() {
    return std::exchange(callee_, CoroutineHandle());
  }

  Task<T> Via(executors::IExecutor& executor) {

  }

 private:
  CoroutineHandle callee_;
  std::string name_;
};

}  // namespace coros::tasks

//////////////////////////////////////////////////////////////////////

template <typename T, typename... Args>
struct std::coroutine_traits<coros::tasks::Task<T>, Args...> {
  using promise_type =
      typename coros::tasks::Task<T>::template Promise<T>;  // NOLINT
};
