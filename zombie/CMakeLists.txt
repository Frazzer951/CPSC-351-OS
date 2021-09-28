cmake_minimum_required(VERSION 3.11.0)
project(Zombie VERSION 1.0.0)

add_executable(zombie zombie.c)

target_clangformat_setup(zombie)