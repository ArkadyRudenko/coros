# ---------- Google Tests ----------
set(CMAKE_CXX_STANDARD 14)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
# --------------

add_executable(main main.cpp)

target_link_libraries(main coros GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(main)

set_target_properties(
        main
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)