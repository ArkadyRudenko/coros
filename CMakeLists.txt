cmake_minimum_required(VERSION 3.25)
project(coros)

include(cmake/Logging.cmake)
include(cmake/CompileOptions.cmake)

option(COROS_TESTS "Build coros tests" ON)
option(COROS_EXAMPLES "Build coros examples" ON)
option(COROS_ADDRESS_SANITIZE "COROS_ADDRESS_SANITIZE on" ON)

add_subdirectory(twist)

add_subdirectory(coros)

if(COROS_ADDRESS_SANITIZE)
    ProjectLog("Enable ADDRESS_SANITIZE")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
#    add_compile_options(-fsanitize=thread)
#    add_link_options(-fsanitize=thread)
endif()

if(COROS_TESTS)
    ProjectLog("COROS_TESTS ARE ENABLED")
    add_subdirectory(tests)
endif()

if(COROS_EXAMPLES)
    ProjectLog("COROS_EXAMPLES ARE ENABLED")
    add_subdirectory(examples)
endif()