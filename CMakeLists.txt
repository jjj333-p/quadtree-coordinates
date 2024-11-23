cmake_minimum_required(VERSION 3.30)
project(program5)

set(CMAKE_CXX_STANDARD 17)

add_executable(program5 main.cpp
        state.h
        quadtree.cpp
        quadtree.h)
