cmake_minimum_required(VERSION 3.11.0)
project(Project_2 VERSION 1.0.0)

add_executable(Fibonacci fibonacci.cpp)
target_link_libraries(Fibonacci pthread)

add_executable(Alphanumeric alphanumeric.cpp)
target_link_libraries(Alphanumeric pthread)

target_clangformat_setup(Fibonacci)
target_clangformat_setup(Alphanumeric)