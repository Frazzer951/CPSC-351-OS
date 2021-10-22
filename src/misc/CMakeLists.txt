cmake_minimum_required(VERSION 3.11.0)
project(Prodcons VERSION 1.0.0)

add_executable(prodcons prodcons.c)
target_link_libraries(prodcons pthread)

target_clangformat_setup(prodcons)