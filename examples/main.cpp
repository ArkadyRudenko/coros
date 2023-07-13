#include <coros/coros.hpp>

#include <iostream>

auto coroutine() -> coros::tasks::Task<int> {
  co_return 42;
}

coros_main({
  std::cout << co_await coroutine();
})