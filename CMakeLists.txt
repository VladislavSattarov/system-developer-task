cmake_minimum_required(VERSION 3.10)
project(SystemDeveloperTask)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

list(APPEND CMAKE_CXX_FLAGS "-Wall -Werror -Wsign-compare")

add_library(intercepting_lib SHARED src/intercepting_lib.cpp src/shared_buffer.cpp)

target_include_directories(intercepting_lib PUBLIC include)
