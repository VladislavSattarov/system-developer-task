cmake_minimum_required(VERSION 3.10)
project(SystemDeveloperTask)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_ASAN "-g -fsanitize=address,undefined -fno-sanitize-recover=all -O2"
    CACHE STRING "Compiler flags in asan build"
    FORCE)

message("General compiler flags: " ${CMAKE_CXX_FLAGS})
message("Debug build compiler flags: " ${CMAKE_CXX_FLAGS_DEBUG})
message("ASAN build compiler flags: " ${CMAKE_CXX_FLAGS_ASAN})

include(FetchContent)

FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.83.0
)

FetchContent_MakeAvailable(boost)

include_directories(${Boost_INCLUDE_DIRS})


add_library(intercepting_lib SHARED src/intercepting_lib.cpp)
target_link_libraries(intercepting_lib ${Boost_LIBRARIES})

target_include_directories(intercepting_lib PUBLIC include)

add_executable(demons src/demons.cpp)
target_link_libraries(demons intercepting_lib)
target_link_libraries(demons ${Boost_LIBRARIES})

install(TARGETS intercepting_lib LIBRARY DESTINATION lib)
install(TARGETS demons RUNTIME DESTINATION bin)
