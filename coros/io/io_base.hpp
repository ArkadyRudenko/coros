#pragma once

#include <coros/tasks/task.hpp>

namespace coros::io {

struct IOBase : public tasks::TaskBase {
  virtual void SetBytes(size_t bytes) noexcept = 0;
};

}