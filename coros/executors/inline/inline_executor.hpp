#pragma once

#include <coros/executors/executor.hpp>

namespace coros::executors {

class InlineExecutor final : public IExecutor {
 public:
  void Execute(tasks::TaskBase* task) override { task->Run(); }
};

IExecutor& Inline() {
  static InlineExecutor instance;
  return instance;
}

}  // namespace coros::executors
