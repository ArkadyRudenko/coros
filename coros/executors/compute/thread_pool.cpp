#include <coros/executors/compute/thread_pool.hpp>
#include <coros/timers/timers_scheduler.hpp>

#include <iostream>

namespace coros::executors::compute {

static thread_local ThreadPool* pool{nullptr};

ThreadPool::ThreadPool(size_t threads)
    : timer_(std::make_unique<timer::TimerScheduler>()),
      io_(std::make_unique<io::IOScheduler>(*this)) {
  for (size_t i = 0; i < threads; ++i) {
    workers_.emplace_back(&ThreadPool::WorkerRoutine, this);
  }
}

void ThreadPool::Execute(tasks::TaskBase* task, Hint hint) {
  if (hint == Hint::AddAction) {
    tasks_count_.Add();
    return;
  }
  if (hint == Hint::RemoveAction) {
    tasks_count_.Done();
    return;
  }
  tasks_count_.Add();
  if (!tasks_.Put(task)) {
    task->Discard();
    std::cerr << "Execute is not ordered with Stop";
    std::abort();
  }
}

void ThreadPool::WorkerRoutine() {
  pool = this;

  while (tasks::TaskBase* next = tasks_.Take()) {
    try {
      next->Run();
    } catch (...) {}
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

ThreadPool* ThreadPool::Current() {
  return pool;
}

ThreadPool::~ThreadPool() {
  Stop();
}

}  // namespace coros::executors::compute