#pragma once

#include <coros/tasks/core/task.hpp>

namespace coros::tasks {

// Blocks current thread
template <typename T>
T Run(Task<T>&& /*task*/) {
  // blocking ~ Await()
}

}  // namespace coros::tasks