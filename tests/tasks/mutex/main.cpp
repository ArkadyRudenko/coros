#include <gtest/gtest.h>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/tasks/core/task.hpp>
#include <coros/tasks/sched/teleport.hpp>
#include <coros/tasks/sched/fire.hpp>
#include <coros/tasks/sched/yield.hpp>
#include <coros/tasks/sync/mutex.hpp>

TEST(Mutex, Yield) {
  using namespace coros;
  executors::compute::ThreadPool scheduler{4};

  tasks::Mutex mutex;
  size_t cs = 0;

  static const size_t kSections = 123456;
  static const size_t kContenders = 17;

  auto contender = [&]() -> tasks::Task<> {
    co_await tasks::TeleportTo(scheduler);

    for (size_t i = 0; i < kSections; ++i) {
      auto lock = co_await mutex.ScopedLock();
      ++cs;
    }
  };

  for (size_t j = 0; j < kContenders; ++j) {
    tasks::FireAndForget(contender());
  }

  scheduler.WaitIdle();

  ASSERT_EQ(cs, kContenders * kSections);

  scheduler.Stop();
}