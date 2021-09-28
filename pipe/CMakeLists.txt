cmake_minimum_required(VERSION 3.11.0)
project(Pipe VERSION 1.0.0)

add_executable(pipe pipe.cpp)
add_executable(double_pipe double_pipe.cpp)

target_clangformat_setup(pipe)
target_clangformat_setup(double_pipe)