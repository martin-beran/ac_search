cmake_minimum_required(VERSION 3.16)

project(ac_search)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-g -O2 -flto -Wall -Wextra -Wswitch-enum -Werror")

include_directories("src/lib")

add_executable(ac_grep src/bin/ac_grep.cpp)
