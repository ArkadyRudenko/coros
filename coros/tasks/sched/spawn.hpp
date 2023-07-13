#pragma once

#include <concepts>

#include <coros/executors/compute/thread_pool.hpp>
#include <coros/tasks/sched/teleport.hpp>
#include <coros/tasks/core/task.hpp>

namespace coros::tasks {

//template <std::invocable Fn>
//void Spawn(Fn&& task) {
//  if constexpr (std::is_same_v<std::invoke_result_t<Fn>, Task<>>) {
//
//  }
//  if (auto* executor = executors::compute::ThreadPool::Current();
//      executor != nullptr) {
//
//  } else {
//    std::cerr << "Spawn() not from executor";
//    std::abort();
//  }
//}

}  // namespace coros::tasks