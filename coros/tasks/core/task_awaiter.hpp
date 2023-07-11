#pragma once

#include <coros/executors/executor.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/task.hpp>

#include <coros/support/unit.hpp>

namespace coros::executors {

namespace detail {

template <typename T>
class TaskAwaiter {
  // TODO:
 public:
  explicit TaskAwaiter(tasks::Task<T>&& task) : task_(std::move(task)) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> h) { handle_ = h; }

  void await_resume() { task_.ReleaseCoroutine().resume(); }

 private:
  std::coroutine_handle<> handle_;
  tasks::Task<T> task_;
};

template <>
class TaskAwaiter<support::Unit> {
 public:
  explicit TaskAwaiter(tasks::Task<support::Unit>&& task) : task_(std::move(task)) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> h) { handle_ = h; }

  void await_resume() { task_.ReleaseCoroutine().resume(); }

 private:
  std::coroutine_handle<> handle_;
  tasks::Task<support::Unit> task_;
};

}  // namespace detail

template <typename T>
auto operator co_await(coros::tasks::Task<T>&& task) {
  return detail::TaskAwaiter{std::move(task)};
}

}  // namespace coros::executors