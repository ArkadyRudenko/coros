#pragma once

#include <coros/tasks/core/task.hpp>

namespace coros::tasks {

inline void FireAndForget(Task<>&& task) {
  task.ReleaseCoroutine().resume();
}

}  // namespace coros::tasks