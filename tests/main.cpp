#include <gtest/gtest.h>

#include <chrono>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/pool_awaiter.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/core/task_awaiter.hpp>
#include <coros/tasks/sched/await.hpp>
#include <coros/tasks/sched/fire.hpp>
#include <coros/tasks/sched/teleport.hpp>

TEST(Main, gorroutine) {
  using namespace coros;

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
}

TEST(Main, teleport) {

  using namespace coros;

  executors::compute::ThreadPool pool{4};

  auto done = false;

  auto gorroutine = [&]() -> tasks::Task<> {
    co_await pool;
    // or explicit teleport:
    // co_await tasks::TeleportTo(pool);

    std::cout << "Hi" << std::endl;
    std::cout << "pool is" << executors::compute::ThreadPool::Current()
              << std::endl;
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

coros::tasks::Task<int> Compute(coros::executors::compute::ThreadPool& pool) {
  co_await pool;
  std::cout << "Step 1" << std::endl;
  std::cout << "th-id Compute: " << std::this_thread::get_id() << std::endl;
  co_return 42;
}

TEST(Main, co_await_in_pool) {
  using namespace coros;
  executors::compute::ThreadPool pool{4};

  auto gorroutine = [&]() -> tasks::Task<> {
    co_await pool;
    std::cout << "th-id Gorroutine: " << std::this_thread::get_id()
              << std::endl;
    int v = co_await Compute(pool) + co_await Compute(pool);
    co_await Compute(pool);
    co_await Compute(pool);
    co_await Compute(pool);
    std::cout << "value = " << v << std::endl;

    co_return;
  };

  auto task = gorroutine();
  tasks::FireAndForget(std::move(task));
  pool.WaitIdle();
  pool.Stop();
}