#pragma once

#include <coros/executors/executor.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/task.hpp>

#include <coros/support/unit.hpp>

namespace coros::tasks {

namespace detail {

template <typename T>
class TaskResultAwaiter {
 public:
  explicit TaskResultAwaiter(tasks::Task<T>&& task) : task_(std::move(task)) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> caller) {
    auto callee = task_.ReleaseCoroutine();
    callee.promise().SetCaller(caller);
    callee.promise().SetResultPlace(&result_);
    callee.resume();  // TODO: push task in pool or task is responsible itself
  }

  T await_resume() { return result_; }

 private:
  T result_;
  tasks::Task<T> task_;
};

template <>
class TaskResultAwaiter<support::Unit> {
 public:
  explicit TaskResultAwaiter(tasks::Task<support::Unit>&& task)
      : task_(std::move(task)) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> caller) {
    auto callee = task_.ReleaseCoroutine();
    callee.promise().SetCaller(caller);
    callee.resume();
  }

  void await_resume() {}

 private:
  tasks::Task<support::Unit> task_;
};

}  // namespace detail

}  // namespace coros::tasks

template <typename T>
auto operator co_await(coros::tasks::Task<T>&& task) {
  return coros::tasks::detail::TaskResultAwaiter{std::move(task)};
}