#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace coros::support {

class TaskCount {
 public:
  void Add(size_t count = 1) {
    tasks_.fetch_add(count, std::memory_order_relaxed);
  }

  void Done(size_t count = 1) {
    if (tasks_.fetch_sub(count, std::memory_order_acq_rel) == count) {
      std::lock_guard lock(mutex_);
      idle_.notify_all();
    }
  }

  void WaitIdle() {
    std::unique_lock lock(mutex_);
    while (tasks_.load() > 0) {
      idle_.wait(lock);
    }
  }

 private:
  std::atomic<size_t> tasks_{0};
  std::mutex mutex_;
  std::condition_variable idle_;
};

}  // namespace coros::support
