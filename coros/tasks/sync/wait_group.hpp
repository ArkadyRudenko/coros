#pragma once

#include <atomic>

#include <coros/support/spinlock.hpp>
#include <coros/tasks/sync/one_shot_event.hpp>

namespace coros::tasks {

class WaitGroup {
 public:
  WaitGroup() { ev_.Lock(); }

  void Add(size_t count) {
    if (count_.fetch_add(count) == 0) {
      ev_.Lock();
    }
  }

  void Done(size_t count = 1) {
    if (count_.fetch_sub(count) == count) {
      ev_.Fire();
    }
  }

  // Asynchronous
  auto Wait() { return ev_.Wait(); }

 private:
  std::atomic<size_t> count_{0};
  OneShotEvent ev_;
};

}  // namespace coros::tasks