#pragma once

#include <coroutine>

#include <coros/executors/executor.hpp>
#include <coros/tasks/awaiter.hpp>
#include <coros/tasks/task.hpp>

namespace coros::tasks {
namespace detail {

class TaskTeleportAwaiter : public tasks::TaskBase, Awaiter<> {
 public:
  TaskTeleportAwaiter(executors::IExecutor& executor) : executor_(executor) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> caller) {
    SetCoroutine(caller);
    executor_.Execute(this);
  }

  void await_resume() {}

  void Run() noexcept override { Resume(); }

  void Discard() noexcept override { Destroy(); }

 private:
  executors::IExecutor& executor_;
};

}  // namespace detail

// Reschedule current coroutine to executor `target`
inline auto TeleportTo(executors::IExecutor& target) {
  return detail::TaskTeleportAwaiter{target};
}

}  // namespace coros::tasks
