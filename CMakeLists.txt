cmake_minimum_required(VERSION 3.11)
project(linked_ptr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS: "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra -pedantic -fsanitize=undefined,address -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/gtest)

add_executable(run-tests
        gtest/gtest-all.cc
        gtest/gtest.h
        linked_ptr.hpp
        tests.cpp main.cpp)

target_link_libraries(run-tests -lpthread)