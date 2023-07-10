#pragma once

#include <coros/tasks/task.hpp>

namespace coros::executors {

struct IExecutor {
  virtual void Execute(tasks::TaskBase* task) = 0;
};

}  // namespace coros::executors
