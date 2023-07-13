#pragma once

#include <coros/tasks/core/task.hpp>

namespace coros::tasks {

// TODO
// Blocks current thread
//template <typename T>
//T Await(Task<T>&& task) {
//  auto handle = task.ReleaseCoroutine();
//  handle.resume();
//  handle.promise();
//  return T{};
//}

}  // namespace coros::tasks