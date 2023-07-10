#pragma once

#include <coroutine>
#include <coros/support/forward_list.hpp>

namespace coros::tasks {

struct ITask {
  virtual ~ITask() = default;

  virtual void Run() noexcept = 0;
  virtual void Discard() noexcept = 0;
};

struct TaskBase : ITask, support::IntrusiveForwardListNode<TaskBase> {};

}  // namespace coros::tasks
