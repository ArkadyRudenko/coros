#pragma once

#include <coros/executors/executor.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/task.hpp>

#include <coros/support/unit.hpp>
#include <coros/support/result.hpp>

namespace coros::tasks {

namespace detail {

template <typename T>
class TaskResultAwaiter : public tasks::TaskBase {
 public:
  explicit TaskResultAwaiter(tasks::Task<T>&& task) : task_(std::move(task)) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> caller) {
    callee_ = task_.ReleaseCoroutine();
    callee_.promise().SetCaller(caller);
    task_.GetExecutor().Execute(this);
  }

  // TODO: Result<T>
  T await_resume() {
    if constexpr (std::is_same_v<T, support::Unit>) {
      return support::Unit{};
    }
    return std::move(callee_.promise().GetResult().ExpectValue());
  }

  void Run() noexcept override { callee_.resume(); }

  void Discard() noexcept override { callee_.destroy(); }

 private:
  using CalleePromise = typename tasks::Task<T>::Promise;
  std::coroutine_handle<CalleePromise> callee_;
  tasks::Task<T> task_;
};

}  // namespace detail

}  // namespace coros::tasks

template <typename T>
auto operator co_await(coros::tasks::Task<T>&& task) {
  return coros::tasks::detail::TaskResultAwaiter{std::move(task)};
}