cmake_minimum_required(VERSION 3.11.0)
project(thread VERSION 1.0.0)

add_executable(thread thread1.c)
target_link_libraries(thread pthread)

add_executable(thread_sum thread_sum.c)
target_link_libraries(thread_sum pthread)

add_executable(multithreading multithreading.c)
target_link_libraries(multithreading pthread)

add_executable(concurrentThreads1 concurrentThreads1.c)
target_link_libraries(concurrentThreads1 pthread)

add_executable(concurrentThreads2 concurrentThreads2.c)
target_link_libraries(concurrentThreads2 pthread)

add_executable(concurrentThreads3 concurrentThreads3.c)
target_link_libraries(concurrentThreads3 pthread)