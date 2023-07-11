#pragma once

#include <coros/tasks/sched/teleport.hpp>

namespace coros::executors {

inline auto operator co_await(IExecutor& executor) {
  return coros::tasks::detail::TaskTeleportAwaiter{executor};
}

}  // namespace coros::executors