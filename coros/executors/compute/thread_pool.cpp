#include <coros/executors/compute/thread_pool.hpp>

namespace coros::executors::compute {

static thread_local ThreadPool* pool{nullptr};

ThreadPool::ThreadPool(size_t threads) {
  for (size_t i = 0; i < threads; ++i) {
    workers_.emplace_back(&ThreadPool::WorkerRoutine, this);
  }
}

void ThreadPool::Execute(tasks::TaskBase* task) {
  tasks_count_.Add(1);
  if (!tasks_.Put(task)) {
    // TODO
  }
}

void ThreadPool::WorkerRoutine() {
  pool = this;

  while (tasks::TaskBase* next = tasks_.Take()) {
    next->Run();
    tasks_count_.Done();
  }
}

void ThreadPool::WaitIdle() {
  tasks_count_.WaitIdle();
}

void ThreadPool::Stop() {
  tasks_.Close();
  for (auto& worker : workers_) {
    worker.join();
  }
  workers_.clear();
}

}  // namespace coros::executors::compute