#include <coros/coros.hpp>

#include <iostream>

int main() {
  using namespace std::chrono_literals;

  coros::RunScheduler([]() -> coros::tasks::Task<> {
    std::cout << "Hello from pool!" << std::endl;

    coros::io::File file = coros::io::File::New("hello.txt", "rw").ExpectValue();

    std::string_view str = "bytes data";
    std::span<std::byte> buffer{(std::byte*)str.data(), str.size()};

    co_await file.Write(buffer);

    std::cout << "After write!\n";

    co_return {};
  });
}