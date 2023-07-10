#pragma once

#include <coros/tasks/sched/teleport.hpp>

namespace coros::tasks {

// Precondition: coroutine is running in `current` executor
inline auto Yield(executors::IExecutor& current) {
  return TeleportTo(current);
}

}  // namespace coros::tasks