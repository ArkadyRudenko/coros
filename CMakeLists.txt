cmake_minimum_required(VERSION 3.25)
project(coros)

set(CMAKE_CXX_STANDARD 20)

option(COROS_TESTS "Build coros tests" ON)
option(COROS_EXAMPLES "Build coros examples" ON)
option(COROS_ADDRESS_SANITIZE "COROS_ADDRESS_SANITIZE on" ON)

add_subdirectory(coros)

if(COROS_ADDRESS_SANITIZE)
    message("Enable ADDRESS_SANITIZE")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
#    add_compile_options(-fsanitize=thread)
#    add_link_options(-fsanitize=thread)
endif()

if(COROS_TESTS)
    message("COROS_TESTS ARE ENABLED")
    add_subdirectory(tests)
endif()

if(COROS_EXAMPLES)
    message("COROS_EXAMPLES ARE ENABLED")
    add_subdirectory(examples)
endif()