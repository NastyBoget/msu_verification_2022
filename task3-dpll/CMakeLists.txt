cmake_minimum_required(VERSION 3.6)
project(task3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I. -Wall -std=c++17")

add_executable(task3 test.cpp)
