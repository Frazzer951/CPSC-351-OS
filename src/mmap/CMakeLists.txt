cmake_minimum_required(VERSION 3.11.0)
project(mMap VERSION 1.0.0)

add_executable(mmap mmap.cpp)

target_clangformat_setup(mmap)