cmake_minimum_required(VERSION 3.16)
project(cpp_development_fundamentals_brown_belt)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(cpp_development_fundamentals_brown_belt main.cpp request.cpp request.h)
