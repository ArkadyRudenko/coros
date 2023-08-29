#pragma once

#include <liburing.h>
#include <cstdio>
#include <cstring>

#include <atomic>
#include <cassert>
#include <optional>
#include <span>
#include <thread>
#include <vector>

#include <coros/executors/executor.hpp>
#include <coros/io/io_action.hpp>

namespace coros::io {

class IOScheduler {
 public:
  IOScheduler(executors::IExecutor& pool);

  void AddAction(IOAction action);
  void Stop();

  ~IOScheduler();

 private:
  void Work();

 private:
  struct io_uring_params params;
  struct io_uring ring;

  executors::IExecutor& pool_;
  std::thread worker_thread_;
  std::atomic<bool> stop_request_{false};
};

}  // namespace coros::io