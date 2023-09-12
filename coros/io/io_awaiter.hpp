#pragma once

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/io/io_action.hpp>
#include <coros/io/io_base.hpp>
#include <coros/io/io_scheduler.hpp>
#include <coros/tasks/awaiter.hpp>

namespace coros::io {

class IOAwaiter : public IOBase, tasks::Awaiter<> {
 public:
  explicit IOAwaiter(IOAction action) : action_(action), read_write_bytes_(0) {
    action_.base = this;
  }

  bool await_ready() { return false; }

  void await_suspend(std::coroutine_handle<> caller) {
    SetCoroutine(caller);
    executors::compute::ThreadPool::Current()->io_->AddAction(action_);
  }

  size_t await_resume() { return read_write_bytes_.load(); }

  void Run() noexcept override { Resume(); }

  void Discard() noexcept override { Destroy(); }

  void SetBytes(size_t bytes) noexcept override {
    read_write_bytes_.store(bytes);
  }

  ~IOAwaiter() override { Destroy(); }

 private:
  IOAction action_;
  std::atomic<size_t> read_write_bytes_;
};

}  // namespace coros::io

auto operator co_await(coros::io::IOAction action) noexcept {
  return coros::io::IOAwaiter{action};
}