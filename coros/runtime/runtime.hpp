#pragma once

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/executor.hpp>
#include <coros/io/io_scheduler.hpp>
#include <coros/timers/timers_scheduler.hpp>

namespace coros {

class Runtime {
 public:
  static Runtime New() { return Runtime{std::thread::hardware_concurrency()}; }

 private:
  Runtime(size_t threads)
      : pool_(std::make_unique<executors::compute::ThreadPool>(threads)),
        io_scheduler_(*pool_) {}

 public:
  std::unique_ptr<executors::IExecutor> pool_;
  timer::TimerScheduler timer_scheduler_;
  io::IOScheduler io_scheduler_;
};

Runtime& Current() {
  static Runtime runtime = Runtime::New();
  return runtime;
}

}  // namespace coros