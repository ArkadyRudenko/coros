#include <gtest/gtest.h>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/pool_awaiter.hpp>
#include <coros/tasks/core/task.hpp>
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

coros::tasks::Task<int> Compute() {
  std::cout << "Step 1" << std::endl;
  co_return 42;
}

TEST(Main, task1) {
  using namespace coros;
  tasks::Task<int> task = Compute();
  auto value = Await(std::move(task));
  std::cout << "value = " << value << std::endl;
}