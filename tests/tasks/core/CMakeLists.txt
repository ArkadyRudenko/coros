add_executable(core main.cpp)

target_link_libraries(core coros GTest::gtest_main)

gtest_discover_tests(core)

set_target_properties(
        core
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)