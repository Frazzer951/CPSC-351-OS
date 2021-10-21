cmake_minimum_required(VERSION 3.11.0)
project(Fork VERSION 1.0.0)

add_executable(fork fork.c)
add_executable(fork_explained fork_explained.c)
add_executable(fork_another1 fork_another1.c)
add_executable(fork_another2 fork_another2.c)

target_clangformat_setup(fork)
target_clangformat_setup(fork_explained)
target_clangformat_setup(fork_another1)
target_clangformat_setup(fork_another2)