cmake_minimum_required(VERSION 3.11.0)
project(Popen VERSION 1.0.0)

add_executable(popen popen.cpp)
add_executable(popen_another popen_another.cpp)

target_clangformat_setup(popen)
target_clangformat_setup(popen_another)