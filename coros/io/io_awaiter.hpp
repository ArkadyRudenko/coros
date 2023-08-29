#pragma once

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/io/io_action.hpp>
#include <coros/io/io_base.hpp>
#include <coros/io/io_scheduler.hpp>

namespace coros::io {

class IOAwaiter : public IOBase {
 public:
  explicit IOAwaiter(IOAction action) : action_(action) { action_.base = this; }

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> caller) {
    caller_ = caller;
    executors::compute::ThreadPool::Current()->io_->AddAction(action_);
  }

  size_t await_resume() { return read_write_bytes_; }

  void Run() noexcept override { caller_.resume(); }

  void Discard() noexcept override {
    caller_.destroy();  // TODO ???
  }

  void SetBytes(size_t bytes) noexcept override { read_write_bytes_ = bytes; }

 private:
  IOAction action_;
  std::coroutine_handle<> caller_;
  size_t read_write_bytes_{0};
};

}  // namespace coros::io

auto operator co_await(coros::io::IOAction action) noexcept {
  return coros::io::IOAwaiter{action};
}