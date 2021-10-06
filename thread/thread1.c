
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


void * thread_func( void * vargp ) { pthread_exit( (void *) 42 ); }

int main()
{
  int       i = 29;
  pthread_t tid_original_thread = 0;
  tid_original_thread = pthread_self();

  pthread_t tid_created_thread = 0;

  printf( " thread id of original thread %lu\n", tid_original_thread );

  pthread_create( &tid_created_thread, NULL, thread_func, NULL );

  printf( " thread id of created thread %lu\n", tid_created_thread );

  pthread_join( tid_created_thread, (void **) &i );

  printf( "%d\n", i );
}
