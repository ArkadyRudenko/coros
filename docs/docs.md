## Base usage

```c++
#include <coros/coros.hpp>

#include <iostream>

auto coroutine() -> coros::tasks::Task<int> {
  co_return 42;
}

int main() {
  coros::RunScheduler([]() -> coros::tasks::Task<> {

    std::cout << "Hello from pool!";

    std::cout << "Value from coroutine = " << co_await coroutine();

    co_return {};
  });
}
```

---

# Design

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
If you want to execute your task in Executor, you must Teleport to executor by _co_await pool_ or with method Via(pool)
```c++

coros::tasks::Task<int> Compute(coros::executors::compute::ThreadPool& pool) {
  co_await pool;
  co_return 42;
}

coros::tasks::Task<int> ComputeWithPool() {
  co_return 42 * 2;
}

int main() {
  using namespace coros;
  executors::compute::ThreadPool pool{4};

  auto gorroutine = [&]() -> tasks::Task<> {
    co_await pool;
    
    // we are in some thread of pool here
    
    int value1 = co_await Compute(pool);
    
    int value2 = co_await ComputeWithPool(pool).Via(pool);
    
    std::cout << "value1 = " << value1  << std::endl;
    std::cout << "value2 = " << value2  << std::endl;
    
    co_return {};
  };

  auto task = gorroutine();
  tasks::FireAndForget(std::move(task));
}
```

Compute(pool) will run later in another (maybe in the same) thread of ThreadPool, so it is async func

gorroutine() is a lazy task. It will be pushed to executor in place where you call tasks::FireAndForget(task) or co_await gorroutine()

You can not use co_await in main() because main is not async func (is ['blue'](https://journal.stuffwithstuff.com/2015/02/01/what-color-is-your-function/) function). For this goal, you should use IExecutor

---
## Timers and IO

### Timers
You can async sleep for some time:
```c++
#include <coros/coros.hpp>

#include <iostream>

int main() {
  coros::RunScheduler([]() -> coros::tasks::Task<> {
    std::cout << "Hello from pool!";

    std::cout << "Start sleep for 5 seconds" << std::endl;

    co_await 5s;

    // or:
    // co_await coros::tasks::SleepFor(5s);
    
    std::cout << "Wake after sleep!" << std::endl;

    co_return {};
  });
}
```

### IO

You can write and read bytes(std::bytes) from files

```c++
#include <coros/coros.hpp>

#include <iostream>

int main() {
  coros::RunScheduler([]() -> coros::tasks::Task<> {
      
    coros::io::File file = coros::io::File::Open("hello.txt", "rw").ExpectValue();

    co_await file.Write("Hello world!\n");

    std::cout << "After write!\n";

    co_return {};
  });
}
```

For std::byte use std::span
```c++
#include <coros/coros.hpp>

#include <iostream>

int main() {
  coros::RunScheduler([]() -> coros::tasks::Task<> {

    coros::io::File file = coros::io::File::Open("hello.txt", "rw").ExpectValue();

    std::string_view str = "bytes data";
    std::span<std::byte> buffer{(std::byte*)str.data(), str.size()};
    
    co_await file.Write(buffer);

    std::cout << "After write!\n";

    co_return {};
  });
}
```