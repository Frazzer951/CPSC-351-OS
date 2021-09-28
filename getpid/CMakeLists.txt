cmake_minimum_required(VERSION 3.11.0)
project(GetPid VERSION 1.0.0)

add_executable(getpid getpid.cpp)

target_clangformat_setup(getpid)