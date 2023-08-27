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
#include <coros/tasks/task.hpp>

namespace coros::io {

enum class Operation {
  Read = 0,
  Write = 1,
};

using Buffer = std::span<std::byte>;

struct IOAction {
  Operation operation;
  Buffer buffer;
  int fd;
  coros::tasks::TaskBase* task;
};

class IOScheduler {
 public:
  IOScheduler(executors::IExecutor& pool)
      : pool_(pool), worker_thread_([this]() { Work(); }) {}

  void AddAction(IOAction action) {
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    sqe->user_data = (__u64)action.task;

    // TODO offsets!
    switch (action.operation) {
      case Operation::Read: {
        io_uring_prep_read(sqe, action.fd, action.buffer.data(),
                           action.buffer.size(), 0);
        break;
      };
      case Operation::Write: {
        io_uring_prep_write(sqe, action.fd, action.buffer.data(),
                            action.buffer.size(), 0);
        break;
      }
    }
    pool_.Execute(nullptr, executors::Hint::AddAction);
    io_uring_submit(&ring);
  }

  void Stop() {
    stop_request_.store(true);
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_write(sqe, 1, "",
                        0, 0);
    io_uring_submit(&ring);
    worker_thread_.join();
  }

  ~IOScheduler() {
    if (!stop_request_) {
      Stop();
    }
  }

 private:
  void Work() {
    memset(&params, 0, sizeof(params));
    int ret = io_uring_queue_init_params(4096, &ring, &params);
    assert(ret >= 0);

    // HERE
    struct io_uring_cqe* cqe = nullptr;
    while (!stop_request_) {

      ret = io_uring_wait_cqe(&ring, &cqe);
      if (stop_request_) {
        break;
      }
      if (ret != 0) {
        std::cout << "Errno = " << errno << std::endl;
      }
      assert(ret == 0);

      assert(cqe->res > 0);

      // Dequeue из очереди CQ.
      io_uring_cqe_seen(&ring, cqe);

      tasks::TaskBase* task = ((tasks::TaskBase*)cqe->user_data);
      pool_.Execute(task);
      pool_.Execute(nullptr, executors::Hint::RemoveAction);
    }
    io_uring_queue_exit(&ring);  // ??
  }

 private:
  struct io_uring_params params;
  struct io_uring ring;

  executors::IExecutor& pool_;
  std::thread worker_thread_;
  std::atomic<bool> stop_request_{false};
};

}  // namespace coros::io