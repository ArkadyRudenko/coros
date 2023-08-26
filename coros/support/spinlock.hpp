#pragma once

#include <atomic>

namespace coros::support {

// Test-and-TAS spinlock

class SpinLock {
 public:
  void Lock() {
    while (true) {
      if (!locked_.exchange(true, std::memory_order_acquire)) {
        break;
      }
      while (locked_.load(std::memory_order_relaxed)) {
        __builtin_ia32_pause();
      }
    }
  }

  void Unlock() { locked_.store(false, std::memory_order_release); }

 private:
  std::atomic<bool> locked_{false};

  // BasicLockable
 public:
  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }
};

}  // namespace exe::support