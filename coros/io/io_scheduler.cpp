#include <coros/io/io_scheduler.hpp>
#include <coros/tasks/task.hpp>

namespace coros::io {

IOScheduler::IOScheduler(executors::IExecutor& pool)
    : pool_(pool), worker_thread_([this]() { Work(); }) {}

void IOScheduler::AddAction(IOAction action) {
  struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  sqe->user_data = (__u64)action.base;

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

void IOScheduler::Stop() {
  stop_request_.store(true);
  struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
  // this operation need to wake worker_thread_ =(
  io_uring_prep_write(sqe, 1, "", 0, 0);
  io_uring_submit(&ring);
  worker_thread_.join();
}

IOScheduler::~IOScheduler() {
  if (!stop_request_) {
    Stop();
  }
}

void IOScheduler::Work() {
  memset(&params, 0, sizeof(params));
  int ret = io_uring_queue_init_params(4096, &ring, &params);
  assert(ret >= 0);

  struct io_uring_cqe* cqe = nullptr;
  while (!stop_request_) {

    ret = io_uring_wait_cqe(&ring, &cqe);
    if (stop_request_) {
      break;
    }
    assert(ret == 0);

    struct io_uring_cqe* cqes[512];
    size_t cqe_count =
        io_uring_peek_batch_cqe(&ring, cqes, sizeof(cqes) / sizeof(cqes[0]));
    for (size_t i = 0; i < cqe_count; ++i) {
      cqe = cqes[i];
      assert(cqe->res > 0); // TODO return Err() instead of assert
      // Dequeue из очереди CQ.
      io_uring_cqe_seen(&ring, cqe);

      tasks::TaskBase* task = ((tasks::TaskBase*)cqe->user_data);
      ((IOBase*)cqe->user_data)->SetBytes(cqe->res);
      pool_.Execute(task);
      pool_.Execute(nullptr, executors::Hint::RemoveAction);
    }
  }
  io_uring_queue_exit(&ring);
}

}  // namespace coros::io
