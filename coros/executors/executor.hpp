#pragma once

#include <coros/tasks/task.hpp>

namespace coros::executors {

enum class Hint {
  UpToYou = 0,
  AddAction = 1, // TODO: this is a poor API
  RemoveAction = 2,
};

struct IExecutor {
  virtual void Execute(tasks::TaskBase* task, Hint hint = Hint::UpToYou) = 0;

  virtual ~IExecutor() = default;
};

}  // namespace coros::executors
