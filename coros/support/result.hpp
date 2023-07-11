#pragma once

#include <memory>

#include <coros/support/error.hpp>

namespace coros::support {

// use https://en.cppreference.com/w/cpp/utility/expected since C++23

template <typename T>
class Result {
 public:
  static Result<T> Ok(T&& value) { return Result<T>{std::move(value)}; }

  template <typename... Args>
  static Result<T> Err(Args&&... args) {
    return Result<T>{std::make_shared<>(std::forward<Args>(args)...)};
  }

 private:
  union value {
    std::shared_ptr<Error> error_;
    T value;
  };
};

}  // namespace coros::support