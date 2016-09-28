cmake_minimum_required(VERSION 2.7)

project(apgame CXX)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -pedantic")
#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wno-error=non-pod-varargs -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG -march=native")

include_directories(include)

add_subdirectory(src)
add_subdirectory(docker)

# used in unittest and profile.
# include(cmake/gtest.cmake)
# include(cmake/benchmark.cmake)
# include(cmake/unittest.cmake)
# include(cmake/profile.cmake)
