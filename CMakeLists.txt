cmake_minimum_required(VERSION 3.16)

project(ac_search)

option(USE_LTO "Use LTO (link time, interprocedural optimization)" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(REMOVE ${CMAKE_SOURCE_DIR}/compile_commands.json)
file(
    CREATE_LINK
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
    SYMBOLIC
)
add_compile_options(
    -Wall -Wextra -pedantic -Werror
    -Wconversion -Wsign-conversion
    -Wswitch-default -Wswitch-enum
    -Wno-mismatched-new-delete
)

if (USE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT CMAKE_INTERPROCEDURAL_OPTIMIZATION)
endif()


include_directories("src/lib")

add_executable(ac_grep src/bin/ac_grep.cpp)
add_executable(ac_benchmark src/bin/ac_benchmark.cpp)
