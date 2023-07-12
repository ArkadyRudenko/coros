#include <coros/coros_main.hpp>
#include <coros/executors/compute/thread_pool.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/sched/fire.hpp>

#include <iostream>

auto coroutine() -> coros::tasks::Task<int> {
  co_return 42;
}

coros_main({
  std::cout << co_await coroutine();
})