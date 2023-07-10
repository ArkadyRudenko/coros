#include <gtest/gtest.h>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/sched/teleport.hpp>
#include <coros/tasks/sched/fire.hpp>

TEST(Main, gorroutine) {

  using namespace coros;

  executors::compute::ThreadPool pool{4};

  auto done = false;

  auto gorroutine = [&]() -> tasks::Task<> {
    std::cout << "Hi" << std::endl;
    done = true;
    co_return;
  };

  auto task = gorroutine();

  ASSERT_FALSE(done);

  tasks::FireAndForget(std::move(task));

  ASSERT_TRUE(done);

  pool.WaitIdle();

  pool.Stop();
}

TEST(Main, teleport) {

  using namespace coros;

  executors::compute::ThreadPool pool{4};

  auto done = false;

  auto gorroutine = [&]() -> tasks::Task<> {
    co_await tasks::TeleportTo(pool);
    std::cout << "Hi" << std::endl;
    std::cout << "pool is" << executors::compute::ThreadPool::Current() << std::endl;
    done = true;
    co_return;
  };

  auto task = gorroutine();

  ASSERT_FALSE(done);

  tasks::FireAndForget(std::move(task));

  pool.WaitIdle();

  ASSERT_TRUE(done);

  pool.Stop();
}