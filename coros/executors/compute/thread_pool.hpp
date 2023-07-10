#pragma once

#include <cstdlib>  // size_t
#include <thread>
#include <vector>

#include <coros/executors/compute/queues/blocking_queue.hpp>
#include <coros/executors/executor.hpp>
#include <coros/support/task_count.hpp>
#include <coros/tasks/task.hpp>

namespace coros::executors::compute {

class ThreadPool : public IExecutor {
 public:
  explicit ThreadPool(size_t threads);

  void Execute(tasks::TaskBase* task) override;

  void WaitIdle();

  void Stop();

  static ThreadPool* Current();

  ~ThreadPool();

 private:
  void WorkerRoutine();

 private:
  std::vector<std::thread> workers_;
  MPMCBlockingQueue<tasks::TaskBase> tasks_;
  support::TaskCount tasks_count_;
};

}  // namespace coros::executors::compute