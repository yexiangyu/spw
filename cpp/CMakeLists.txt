cmake_minimum_required(VERSION 3.10)

project(spw)

add_library(spw SHARED spw.cpp)

add_executable(demo demo.c)
target_link_libraries(demo PRIVATE spw)