#pragma once

#include <cstdlib>  // size_t
#include <thread>
#include <vector>

#include <coros/executors/compute/queues/blocking_queue.hpp>
#include <coros/executors/executor.hpp>
#include <coros/support/task_count.hpp>
#include <coros/tasks/task.hpp>

namespace coros::tasks {
template <typename Rep, typename Period>
class TimerAwaiter;
}

namespace coros::timer {
class TimerScheduler;
}

namespace coros::executors::compute {

class ThreadPool : public IExecutor {
  template <typename Rep, typename Period>
  friend class tasks::TimerAwaiter;
  friend class timer::TimerScheduler;
 public:
  explicit ThreadPool(size_t threads);

  void Execute(tasks::TaskBase* task, Hint hint = Hint::UpToYou) override;

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
  std::unique_ptr<timer::TimerScheduler> timer_;
};

}  // namespace coros::executors::compute