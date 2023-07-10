#include <gtest/gtest.h>

#include <coros/executors/compute/thread_pool.hpp>

TEST(Main, test1) {

  coros::executors::compute::ThreadPool pool{4};

  pool.WaitIdle();

  pool.Stop();

  ASSERT_EQ(1, 1);
}