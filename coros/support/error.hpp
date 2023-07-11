#pragma once

#include <string>

namespace coros::support {

struct Error {
  virtual std::string Description() const = 0;
};

}  // namespace coros::support