#pragma once

#include <type_traits>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/pool_awaiter.hpp>
#include <coros/executors/shard/thread_pool.hpp>
#include <coros/io/file.hpp>
#include <coros/io/io_awaiter.hpp>
#include <coros/io/io_scheduler.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/core/task_awaiter.hpp>
#include <coros/tasks/sched/fire.hpp>
#include <coros/tasks/sched/timer_awaiter.hpp>
#include <coros/tasks/task.hpp>
#include <coros/timers/timer.hpp>

namespace coros {

template <std::invocable Func>
  requires requires(Func func) {
    std::same_as<decltype(func()), coros::tasks::Task<>>;
  }
void RunScheduler(Func func) {
  coros::executors::compute::ThreadPool pool{
      std::thread::hardware_concurrency()};

  auto main = [&]() -> coros::tasks::Task<> {
    co_await pool;

    co_await func();

    co_return {};
  };

  coros::tasks::FireAndForget(main());

  pool.WaitIdle();

  pool.Stop();
}

};  // namespace coros