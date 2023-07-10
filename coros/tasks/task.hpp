#pragma once

#include <coros/support/forward_list.hpp>
#include <coroutine>

namespace coros::tasks {

struct ITask {
  virtual ~ITask() = default;

  virtual void Run() noexcept = 0;
  virtual void Discard() noexcept = 0;
};

struct TaskBase : ITask, support::IntrusiveForwardListNode<TaskBase> {};

}  // namespace coros::tasks
