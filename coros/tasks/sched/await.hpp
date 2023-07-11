#pragma once

#include <coros/tasks/core/task.hpp>

namespace coros::tasks {

// Blocks current thread
template <typename T>
T Await(Task<T>&& task) {
  auto handle = task.ReleaseCoroutine();
  while (!handle.done()) {
    handle.resume();
  }
  return std::move(handle.promise().GetResult().value());
}

}  // namespace coros::tasks