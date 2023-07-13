#pragma once

#include <exception>
#include <variant>

#include <coros/support/unit.hpp>

namespace coros::support {

class Error {
 public:
  Error() : error_(Unit{}) {}

  Error(std::exception_ptr e) : error_(std::move(e)) {}

  bool HasException() const {
    return std::holds_alternative<std::exception_ptr>(error_);
  }

  void ThrowIfError() const {
    if (HasException()) {
      std::rethrow_exception(std::get<std::exception_ptr>(error_));
    }
  }

 private:
  std::variant<Unit, std::exception_ptr> error_;
};

}  // namespace coros::support