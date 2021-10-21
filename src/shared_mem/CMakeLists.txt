cmake_minimum_required(VERSION 3.11.0)
project(Shared_Mem VERSION 1.0.0)

add_executable(shared_mem_reader shared_mem_reader.c)
add_executable(shared_mem_writer shared_mem_writer.c)

target_clangformat_setup(shared_mem_reader)
target_clangformat_setup(shared_mem_writer)