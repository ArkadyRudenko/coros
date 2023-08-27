#pragma once

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/pool_awaiter.hpp>
#include <coros/tasks/task.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/core/task_awaiter.hpp>
#include <coros/tasks/sched/fire.hpp>
#include <coros/io/io_scheduler.hpp>
#include <coros/io/io_awaiter.hpp>

#define coros_main(main_func)                         \
int main() {                                          \
  coros::executors::compute::ThreadPool pool{8};      \
  auto main_coro = [&]()-> coros::tasks::Task<> {     \
    co_await pool;                                    \
    main_func;                                        \
    co_return {};                                     \
  };                                                  \
  coros::tasks::FireAndForget(std::move(main_coro()));\
  pool.WaitIdle();                                    \
  pool.Stop();                                        \
  return 0;                                           \
}