#pragma once

#include <cassert>
#include <span>
#include <type_traits>

#include <coros/tasks/task.hpp>
#include <coros/io/io_base.hpp>

namespace coros::io {

enum class Operation {
  Read = 0,
  Write = 1,
};

using Buffer = std::span<std::byte>;

template <typename T>
  requires((std::same_as<typename T::value_type, std::byte> ||
            std::same_as<typename T::value_type, char>) &&
           requires(T t) {
             t.data();
             t.size();
             t.capacity();
           })
Buffer ToBuffer(T& t) {
  return {(std::byte*)t.data(), t.capacity()}; // TODO: make class Bytes
}

struct IOAction {
  Operation operation;
  Buffer buffer;
  int fd;
  io::IOBase* base;
};

}  // namespace coros::io