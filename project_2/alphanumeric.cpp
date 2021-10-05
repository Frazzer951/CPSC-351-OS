#include <pthread.h>

/*
PART B: Printing Alpha Numeric Words [ 100 points ]
Write a C++ program (using the pthread library) that accepts a phrase of unspecified length on the command line. For example:
prompt$: ./alphanumeric “Operating Systems 351 begins at 1:00pm or 4:00pm”
The main() in this program should read the phrase from the terminal. This phrase can be read into a global variable. This phrase or its parts can be directly accessed from the main() and from the threads. The main()  has to create two concurrent threads running functions (alpha and numeric). The threads should print only the words of the phrase as follows:
The alpha thread function should print all the words that start with an alphabet.
The numeric thread function should print all the words starting with a number.
Note that the main thread (running main()) should not print anything itself, the output should be printed by the threads that it creates. The main() can decide which thread to begin executing first, or it can leave it to the threads to sort this out. The order of words in the phrase should not be changed in the print. Your program should work for any phrase of any reasonable length, not just the one given in the example. The output of your program should look similar to the following
$: ./alphanumeric “Operating Systems 351 begins at 1:00pm or 4:00pm”
alpha: Operating
alpha: Systems
numeric: 351
alpha: begins
alpha: at
numeric: 1:00pm
alpha: or
numeric: 4:00pm
In this part you are *not allowed* to use ANY synchronization primitives including but not limited to: semaphores, mutexes, mutex locks such as pthread_mutex_lock and pthread_mutex_unlock and conditional variables such as pthread_cond_wait and pthread_cond_signal etc. from pthreads library for thread coordination.
You DO NOT NEED to use sched_yield() to relinquish control of the CPU, but you can use it if you like. You may also investigate some of the other pthread functions available to you for this project, and use them if you like. Although you do not actually need them.
The name of this program must be alphanumeric.cpp
*/

int main( int argc, char * argv[] ) { return 0; }