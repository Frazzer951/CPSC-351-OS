cmake_minimum_required(VERSION 3.11.0)
project(Project_3 VERSION 1.0.0)

add_executable(file_sys_example file_sys_example.cpp)
add_executable(mmap mmap.cpp)

target_clangformat_setup(file_sys_example)
target_clangformat_setup(mmap)