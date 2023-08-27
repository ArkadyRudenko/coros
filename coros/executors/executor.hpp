#pragma once

#include <coros/tasks/task.hpp>

namespace coros::executors {

enum class Hint {
  UpToYou = 0,
  AddAction = 1,
  RemoveAction = 2,
};

struct IExecutor {
  virtual void Execute(tasks::TaskBase* task, Hint hint = Hint::UpToYou) = 0;
};

}  // namespace coros::executors
