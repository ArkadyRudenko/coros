#pragma once

#include <coros/tasks/core/task.hpp>

namespace coros::tasks {

// Blocks current thread
template <typename T>
T Await(Task<T>&& task) {
  auto handle = task.ReleaseCoroutine();
  handle.resume();
  return std::move(handle.promise().result.value());
}

}  // namespace coros::tasks