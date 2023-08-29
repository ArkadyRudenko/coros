#pragma once

#include <coros/executors/executor.hpp>

namespace coros::executors::shard {

class ThreadPool : public IExecutor {
 public:
  void Execute(tasks::TaskBase* task, Hint hint = Hint::UpToYou) override;

 private:
};

}