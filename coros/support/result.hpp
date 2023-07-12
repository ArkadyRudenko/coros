#pragma once

#include <memory>

namespace coros::support {

// use https://en.cppreference.com/w/cpp/utility/expected since C++23

// TODO all
template <typename T, typename Err>
class Result {
 public:
  static Result<T, Err> Ok(T&& value) {
    return Result<T, Err>{std::move(value)};
  }

  static Result<T, Err> Error(Err&& error) {
    return Result<T, Err>{std::move(error)};
  }

  bool IsErr() {}

 private:

 private:
  Err error_;
  T value;
};

}  // namespace coros::support