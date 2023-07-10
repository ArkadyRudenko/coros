add_executable(wait_group main.cpp)

target_link_libraries(wait_group coros GTest::gtest_main)

gtest_discover_tests(wait_group)

set_target_properties(
        wait_group
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)