#pragma once

#include <fcntl.h>
#include <string>
#include <string_view>

#include <coros/io/io_scheduler.hpp>
#include <coros/support/result.hpp>
#include <utility>

namespace coros::io {

using support::Result;

class File {
 public:
  static Result<File> New(std::string_view name, std::string_view flags) {
    int flags_ready = O_CREAT | O_APPEND;
    if (IsRead(flags) && IsWrite(flags)) {
      flags_ready = flags_ready | O_RDWR;
    } else if (IsRead(flags)) {
      flags_ready = flags_ready | O_RDONLY;
    } else if (IsWrite(flags)) {
      flags_ready = flags_ready | O_WRONLY;
    }

    int fd = open(name.data(), flags_ready, 0666);  // TODO: remove all permission

    if (fd == -1) {
      return Result<File>::Err(support::Error{});
    }
    return Result<File>::Ok(
        File(std::move(std::string(name)), flags_ready, fd));
  }

  IOAction Read(Buffer buffer) {
    return IOAction{Operation::Read, buffer, fd_};
  }

  IOAction Write(Buffer buffer) {
    return IOAction{Operation::Write, buffer, fd_};
  }

  IOAction Write(std::string_view str) {
    return IOAction{
        Operation::Write, {(std::byte*)str.data(), str.size()}, fd_};
  }

  // TODO: better API

  File WriteOnly() && {
    flags_ = flags_ | O_WRONLY;
    return std::move(*this);
  }

  File ReadOnly() && {
    flags_ = flags_ | O_RDONLY;
    return std::move(*this);
  }

  File ReadWrite() && {
    flags_ = flags_ | O_RDWR;
    return std::move(*this);
  }

 private:
  File(std::string&& name, int flags, int fd)
      : name_(std::move(name)), flags_(flags), fd_(fd) {}

  static bool IsRead(std::string_view flags) {
    return flags.find('r') != std::string_view::npos;
  }

  static bool IsWrite(std::string_view flags) {
    return flags.find('w') != std::string_view::npos;
  }

 private:
  std::string name_;
  int flags_;
  int fd_;
};

}  // namespace coros::io