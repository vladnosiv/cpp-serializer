cmake_minimum_required(VERSION 3.17)
project(serializer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Sources src/main.cpp)
set(Headers src/serializer.hpp)
add_executable(serializer ${Sources} ${Headers})