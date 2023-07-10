#pragma once

#include <coroutine>

#include <coros/executors/executor.hpp>
#include <coros/tasks/task.hpp>

namespace coros::tasks {
namespace detail {

class TaskAwaiter : public tasks::TaskBase {
 public:
  TaskAwaiter(executors::IExecutor& executor) : executor_(executor) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> h) {
    handle_ = h;
    executor_.Execute(this);
  }

  void await_resume() {}

  void Run() noexcept override { handle_.resume(); }

  void Discard() noexcept override { handle_.destroy(); }

 private:
  std::coroutine_handle<> handle_;
  executors::IExecutor& executor_;
};

}  // namespace detail

// Reschedule current coroutine to executor `target`
inline auto TeleportTo(executors::IExecutor& target) {
  return detail::TaskAwaiter{target};
}
}  // namespace coros::tasks