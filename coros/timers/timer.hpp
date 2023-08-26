#pragma once

#include <chrono>

#include <coros/support/forward_list.hpp>

namespace coros::timer {

using Millis = std::chrono::milliseconds;

template <typename Dur>
inline Millis ToMillis(Dur dur) {
  return std::chrono::duration_cast<Millis>(dur);
}

struct ITimer {
  virtual ~ITimer() = default;

  virtual Millis Delay() const = 0;

  virtual void Alarm() = 0;
};

struct TimerBase : ITimer,
                   public support::IntrusiveForwardListNode<TimerBase> {};

}  // namespace coros::timer