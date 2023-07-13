## Base usage

```c++
#include <coros/coros.hpp>

#include <iostream>

auto coroutine() -> coros::tasks::Task<int> {
  co_return 42;
}

coros_main({
  std::cout << co_await coroutine();
})
```

---

coros consists of two main components:
  - executors: [ThreadPool](../coros/executors/compute/thread_pool.hpp)
  - tasks: [Task](../coros/tasks/task.hpp)

[Executors](../coros/executors/executor.hpp) execute your tasks(coroutines)

You can make a simple coroutine with defining return type as [coros::tasks::Task<>](../coros/tasks/core/task.hpp)
```c++
auto coroutine() -> coros::tasks::Task<int> {
  co_return 42;
}
```
If you want to execute your task in Executor, you must Teleport to executor by _co_await pool_
```c++

coros::tasks::Task<int> Compute(coros::executors::compute::ThreadPool& pool) {
  co_await pool;
  co_return 42;
}

int main() {
  using namespace coros;
  executors::compute::ThreadPool pool{4};

  auto gorroutine = [&]() -> tasks::Task<> {
    co_await pool;
    int value = co_await Compute(pool);
    std::cout << "value = " << value << std::endl;
    co_return;
  };

  auto task = gorroutine();
  tasks::FireAndForget(std::move(task));
}
```

Compute(pool) will run later in another (maybe in the same) thread of ThreadPool, so it is async func
gorroutine() is a lazy task. It will be pushed to executor in place where you call tasks::FireAndForget(task) or co_await gorroutine()

You can not use co_await in main() because main is not async func (is 'blue' function). For this goal, you should use IExecutor