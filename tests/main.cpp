#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/pool_awaiter.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/core/task_awaiter.hpp>
#include <coros/tasks/sched/await.hpp>
#include <coros/tasks/sched/fire.hpp>
#include <coros/tasks/sched/teleport.hpp>
#include <coros/tasks/sched/timer.hpp>

TEST(Main, Gorroutine) {
  using namespace coros;

  auto done = false;

  auto gorroutine = [&]() -> tasks::Task<> {
    std::cout << "Hi" << std::endl;
    done = true;
    co_return{};
  };

  auto task = gorroutine();

  ASSERT_FALSE(done);

  tasks::FireAndForget(std::move(task));

  ASSERT_TRUE(done);
}

TEST(Main, Teleport) {

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
    co_return{};
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

TEST(Main, CoAwaitInPool) {
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

    co_return{};
  };

  auto task = gorroutine();
  tasks::FireAndForget(std::move(task));
  pool.WaitIdle();
  pool.Stop();
}

coros::tasks::Task<int> Compute() {
  std::cout << "th-id Compute: " << std::this_thread::get_id() << std::endl;
  co_return 42;
}

TEST(Main, Via) {
  using namespace coros;
  executors::compute::ThreadPool pool{4};

  auto gorroutine = [&]() -> tasks::Task<> {
    std::cout << "th-id before teleport: " << std::this_thread::get_id()
              << std::endl;
    co_await pool;
    std::cout << "th-id Gorroutine: " << std::this_thread::get_id()
              << std::endl;
    int v = co_await Compute().Via(pool);
    int v2 = co_await Compute();
    std::cout << "value = " << v + v2 << std::endl;
    co_return{};
  };

  //  coros::tasks::Await(gorroutine());
  tasks::FireAndForget(gorroutine());
  pool.WaitIdle();
  pool.Stop();
}

TEST(Main, Timer) {
  using namespace coros;
  using namespace std::chrono_literals;

  executors::compute::ThreadPool pool{4};

  auto timer = [&]() -> tasks::Task<> {
    co_await pool;

    std::cout << "Start sleep for 3 seconds..." << std::endl;

    co_await 3s;

    std::cout << "3s later..." << std::endl;

    co_return{};
  };

  tasks::FireAndForget(timer());
  std::this_thread::sleep_for(7s);

  pool.WaitIdle();

  pool.Stop();
}