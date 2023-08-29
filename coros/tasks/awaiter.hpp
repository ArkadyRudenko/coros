#pragma once

#include <coroutine>
#include <utility>

#include <coros/support/unit.hpp>

namespace coros::tasks {

template <typename T = void>
struct Awaiter {
  using CoroutineHandle = std::coroutine_handle<T>;

  void SetCoroutine(CoroutineHandle caller) { caller_ = caller; }

  void Resume() {
    if (caller_) {
      ReleaseCoroutine().resume();
    } else {
      std::cerr << "Coroutine is not set\n";
      std::abort();
    }
  }

  void Destroy() {
    if (caller_) {
      ReleaseCoroutine().destroy();
    }
  }

 private:
  CoroutineHandle ReleaseCoroutine() {
    return std::exchange(caller_, CoroutineHandle());
  }

 private:
  CoroutineHandle caller_;
};

}  // namespace coros::tasks