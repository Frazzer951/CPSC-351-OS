cmake_minimum_required(VERSION 3.11.0)
project(MSQueue VERSION 1.0.0)

add_executable(msqueue_receiver msqueue_receiver.c)
add_executable(msqueue_sender msqueue_sender.c)

target_clangformat_setup(msqueue_receiver)
target_clangformat_setup(msqueue_sender)