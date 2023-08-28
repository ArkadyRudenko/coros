#include <coros/coros.hpp>

#include <iostream>

int main() {
  using namespace std::chrono_literals;

  coros::RunScheduler([]() -> coros::tasks::Task<> {
    std::cout << "Hello from pool!" << std::endl;

    coros::io::File file =
        coros::io::File::Open("hello.txt", "rw").ExpectValue();

    std::string_view str = "bytes data";
    coros::io::Buffer buffer = coros::io::ToBuffer(str);

    co_await file.Write(buffer);

    std::cout << "After write!\n";

    co_return {};
  });
}