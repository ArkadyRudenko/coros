set(LIB_TARGET coros)

add_compile_options("-fcoroutines-ts")

get_filename_component(LIB_PATH "." ABSOLUTE)
get_filename_component(LIB_INCLUDE_PATH ".." ABSOLUTE)

file(GLOB_RECURSE LIB_CXX_SOURCES ${LIB_PATH}/*.cpp)
file(GLOB_RECURSE LIB_HEADERS ${LIB_PATH}/*.hpp)

add_library(${LIB_TARGET} STATIC ${LIB_CXX_SOURCES} ${LIB_HEADERS})
target_include_directories(${LIB_TARGET} PUBLIC ${LIB_INCLUDE_PATH})

target_link_libraries(${LIB_TARGET} PUBLIC "/lib/liburing.a")

set_target_properties(
        ${LIB_TARGET}
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)
