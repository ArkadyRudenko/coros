#pragma once

#include <atomic>

#include <coros/support/spinlock.hpp>
#include <coros/tasks/sync/one_shot_event.hpp>

namespace coros::tasks {

class WaitGroup {
 public:
  void Add(size_t count) {
    std::lock_guard lock(spinlock_); // TODO
    count_.fetch_add(count);
  }

  void Done() {
    std::unique_lock lock(spinlock_); // TODO
    if (count_.fetch_sub(1) == 1) {
      one_shot_event_.Fire(lock);
    }
  }

  // Asynchronous
  auto Wait() { return one_shot_event_.Wait(); }

 private:
  std::atomic<size_t> count_{0};
  exe::support::SpinLock spinlock_;
  OneShotEvent one_shot_event_{count_, spinlock_};
};

}  // namespace coros::tasks