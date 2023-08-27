#pragma once

#include <coros/executors/executor.hpp>
#include <coros/executors/compute/thread_pool.hpp>
#include <coros/tasks/task.hpp>
#include <coros/io/io_scheduler.hpp>

namespace coros::io {

class IOAwaiter : public tasks::TaskBase {
 public:
  IOAwaiter(IOAction action) : action_(action) {}

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> caller) {
    caller_ = caller;
    executors::compute::ThreadPool::Current()->io_->AddAction(action_);
  }

  void await_resume() {}

  void Run() noexcept override {
    caller_.resume();
  }

  void Discard() noexcept override {
    caller_.destroy(); // TODO ???
  }

 private:
  IOAction action_;
  std::coroutine_handle<> caller_;
};

}  // namespace coros::io

auto operator co_await(coros::io::IOAction action) noexcept {
  return coros::io::IOAwaiter{action};
}