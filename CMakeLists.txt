cmake_minimum_required(VERSION 3.28..3.5)

project(compiler)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

add_executable(cmp src/main.cpp)