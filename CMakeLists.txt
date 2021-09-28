cmake_minimum_required(VERSION 3.11.0)
project(CPCS_351_OS VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(ClangFormat)

file(GLOB_RECURSE files
     *.cpp
     *.hpp
)

add_subdirectory(project_1)
add_subdirectory(thread)

clangformat_setup(
  ${files}
)