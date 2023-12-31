#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/executors/pool_awaiter.hpp>
#include <coros/io/file.hpp>
#include <coros/io/io_awaiter.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/core/task_awaiter.hpp>
#include <coros/tasks/core/background_awaiter.hpp>
#include <coros/tasks/sched/await.hpp>
#include <coros/tasks/sched/fire.hpp>
#include <coros/tasks/sched/teleport.hpp>
#include <coros/tasks/sched/timer_awaiter.hpp>
#include <coros/tasks/sync/mutex.hpp>

TEST(Main, Gorroutine) {
  using namespace coros;

  auto done = false;

  auto gorroutine = [&]() -> tasks::Task<> {
    std::cout << "Hi" << std::endl;
    done = true;
    co_return {};
  };

  auto task = gorroutine();

  ASSERT_FALSE(done);

  tasks::FireAndForget(task);

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
    co_return {};
  };

  auto task = gorroutine();

  ASSERT_FALSE(done);

  tasks::FireAndForget(task);

  pool.WaitIdle();

  ASSERT_TRUE(done);

  pool.Stop();
}

coros::tasks::Task<int> Compute() {
  std::cout << "th-id Compute: " << std::this_thread::get_id() << std::endl;
  co_return 42;
}

TEST(Main, CoAwaitInPool) {
  using namespace coros;
  executors::compute::ThreadPool pool{4};

  auto gorroutine = [&]() -> tasks::Task<> {
    std::cout << "Start th-id Gorroutine: " << std::this_thread::get_id()
              << std::endl;
    co_await pool;
    std::cout << "th-id Gorroutine: " << std::this_thread::get_id()
              << std::endl;
    int v = co_await Compute().Via(pool) + co_await Compute().Via(pool);
    co_await Compute().Via(pool);
    std::cout << "value = " << v << std::endl;
    std::cout << "Final" << std::endl;

    co_return {};
  };

  auto task = gorroutine();
  tasks::FireAndForget(task);
  pool.WaitIdle();
  pool.Stop();
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
    co_return {};
  };

  //  coros::tasks::Await(gorroutine());
  auto task = gorroutine();
  tasks::FireAndForget(task);
  pool.WaitIdle();
  pool.Stop();
}

template <std::invocable Fn>
auto CreateTask(Fn&& task) {
  return std::move(task());
}

TEST(Main, Timer) {
  using namespace coros;
  using namespace coros::tasks;
  using namespace std::chrono_literals;

  executors::compute::ThreadPool pool{4};

  auto timer = [&]() -> tasks::Task<> {
    co_await pool;
//    for (size_t i = 0; i < 5; ++i) {
//      co_await BackgroundAwaiter<>([]() -> Task<> {
//                    co_await SleepFor(1s);
//                    std::cout << "I`m wake!" << std::endl;
//                    co_return {};
//                  }().Via(pool));
//    }
    std::cout << "Start sleep for 3 seconds..." << std::endl;
    co_await 3s;
    std::cout << "3s later..." << std::endl;
    co_await 3s;
    std::cout << "3s later..." << std::endl;
    co_return {};
  };

  auto task = timer();
  tasks::FireAndForget(task);
  pool.WaitIdle();
  pool.Stop();
}

TEST(Main, Mutex) {
  using namespace coros;
  using namespace std::chrono_literals;
  using namespace tasks;

  executors::compute::ThreadPool pool{4};

  size_t counter{0};

  static const size_t tasks_count = 1000;
  static const size_t iter_count = 1000;

  auto main = [&]() -> Task<> {
    co_await pool;

    Mutex mutex;

    for (size_t i = 0; i < tasks_count; ++i) {
      co_await [&]() -> Task<> {
        for (size_t j = 0; j < iter_count; ++j) {
          co_await mutex.ScopedLock();
          ++counter;
        }
        co_return {};
      }().Via(pool);
    }

    co_return {};
  };

  auto task = main();
  tasks::FireAndForget(task);

  pool.WaitIdle();

  pool.Stop();

  ASSERT_EQ(counter, tasks_count * iter_count);
}

TEST(Main, IO) {
  using namespace coros;
  using namespace std::chrono_literals;
  using namespace tasks;

  executors::compute::ThreadPool pool{4};

  auto main = [&]() -> Task<> {
    co_await pool;

    io::File file = io::File::Open("hello.txt", "rw").ExpectValue();

    std::string_view hello = "Hello!\n";

    size_t bytes = co_await file.Write(hello);

    std::cout << "write = " << bytes << std::endl;

    std::vector<std::byte> out;
    out.reserve(20);

    bytes = co_await file.Read(io::ToBuffer(out));

    std::cout << "read = " << bytes << std::endl;

    const char* res = (const char*)out.data();
    for (size_t i = 0; i < bytes; ++i) {
      std::cout << res[i];
    }

    co_return {};
  };

  auto task = main();
  tasks::FireAndForget(task);

  pool.WaitIdle();

  pool.Stop();
}