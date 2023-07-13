#pragma once

#include <memory>

#include <coros/support/error.hpp>

namespace coros::support {

// use https://en.cppreference.com/w/cpp/utility/expected since C++23

template <typename T>
class Result {
 public:
  static Result<T> Ok(T&& value) { return Result<T>{std::move(value)}; }

  static Result<T> Err(Error&& error) { return Result<T>{std::move(error)}; }

  [[nodiscard]] bool IsErr() const {
    return std::holds_alternative<Error>(value_);
  }

  [[nodiscard]] bool IsOk() const { return std::holds_alternative<T>(value_); }

  T GetValue() { return std::move(std::get<T>(value_)); }

  Error GetErr() { return std::move(std::get<Error>(value_)); }

  T ExpectValue() {
    if (IsErr()) {
      GetErr().ThrowIfError();
      throw std::runtime_error("result is empty");
    } else if (IsOk()) {
      return std::move(GetValue());
    }
    throw std::runtime_error("result is empty");
  }

 private:
  explicit Result(T&& value) : value_(std::move(value)) {}

  explicit Result(Error&& error) : value_(std::move(error)) {}

 private:
  std::variant<T, Error> value_;
};

}  // namespace coros::support