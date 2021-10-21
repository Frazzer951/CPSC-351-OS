#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int j = 0;

void * thread_func( void * arg )
{
  for( int i = 0; i < 100; i++ ) { j++; }

  printf( "j is %d\n", j );

  pthread_exit( 0 );
}

int main( int argc, char * argv[] )
{
  pthread_t p1 = 0;
  pthread_t p2 = 0;

  pthread_create( &p1, NULL, thread_func, NULL );
  pthread_create( &p2, NULL, thread_func, NULL );

  pthread_join( p1, NULL );
  pthread_join( p2, NULL );
}
