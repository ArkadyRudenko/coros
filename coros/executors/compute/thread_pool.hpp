#pragma once

#include <cstdlib>  // size_t
#include <thread>
#include <vector>

#include <coros/executors/compute/queues/blocking_queue.hpp>
#include <coros/executors/executor.hpp>
#include <coros/support/task_count.hpp>
#include <coros/tasks/task.hpp>
//#include <coros/tasks/sched/timer.hpp>
#include <coros/timers/timers_scheduler.hpp>

namespace coros::tasks {
template <typename Rep, typename Period>
class TimerAwaiter;
}

namespace coros::executors::compute {

class ThreadPool : public IExecutor {
  template <typename Rep, typename Period>
  friend class tasks::TimerAwaiter;

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
  timer::TimerScheduler timer_;
};

}  // namespace coros::executors::compute