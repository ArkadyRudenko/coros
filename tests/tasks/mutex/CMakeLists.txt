add_executable(mutex main.cpp)

target_link_libraries(mutex coros GTest::gtest_main)

gtest_discover_tests(mutex)

set_target_properties(
        mutex
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)