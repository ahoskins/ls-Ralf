cmake_minimum_required(VERSION 3.3)
project(ls_ralf)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c main.h)
add_executable(ls_ralf ${SOURCE_FILES})