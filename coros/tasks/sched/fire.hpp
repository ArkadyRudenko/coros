#pragma once

#include <coros/tasks/core/task.hpp>

namespace coros::tasks {

inline void FireAndForget(Task<>& task) {
  task.GetCoroutine().resume();
}

}  // namespace coros::tasks