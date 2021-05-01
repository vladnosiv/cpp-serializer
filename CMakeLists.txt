cmake_minimum_required(VERSION 3.17)
project(serializer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(serializer src/serializer.hpp  src/main.cpp)