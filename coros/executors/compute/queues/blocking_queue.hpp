#pragma once

#include <condition_variable>
#include <mutex>

#include <coros/support/forward_list.hpp>

namespace coros::executors::compute {
template <typename T>
class MPMCBlockingQueue {
 public:
  // Returns false iff queue is closed for producers
  bool Put(T* item) {
    {
      std::lock_guard lock(mutex_);
      if (closed_) {
        return false;
      }
      items_.PushBack(item);
    }
    not_empty_.notify_one();
    return true;
  }

  // Await and take next item
  // Returns nullptr iff queue is both 1) drained and 2) closed
  T* Take() {
    std::unique_lock lock(mutex_);
    while (items_.IsEmpty() && !closed_) {
      not_empty_.wait(lock);
    }
    return items_.PopFront();
  }

  // Close queue for producers
  void Close() {
    std::lock_guard lock(mutex_);
    closed_ = true;
    not_empty_.notify_all();
  }

 private:
  support::IntrusiveForwardList<T> items_;
  bool closed_{false};
  std::mutex mutex_;
  std::condition_variable not_empty_;
};

}  // namespace coros::executors::compute