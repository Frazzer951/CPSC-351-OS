cmake_minimum_required(VERSION 3.11.0)
project(Thread VERSION 1.0.0)

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

add_executable(threadTest threadTest.c)
target_link_libraries(threadTest pthread)

target_clangformat_setup(thread)
target_clangformat_setup(thread_sum)
target_clangformat_setup(multithreading)
target_clangformat_setup(concurrentThreads1)
target_clangformat_setup(concurrentThreads2)
target_clangformat_setup(concurrentThreads3)
target_clangformat_setup(threadTest)