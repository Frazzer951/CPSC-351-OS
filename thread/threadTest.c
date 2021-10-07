#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

volatile int balance = 0;

void * mythread( void * arg )
{
  int i = 0;

  for( i = 0; i < 200; i++ ) { balance++; }

  printf( "Balance is %d\n", balance );

  return NULL;
}

int main( int argc, char * argv[] )
{
  pthread_t p1;
  pthread_t p2;
  pthread_t p3;

  pthread_create( &p1, NULL, mythread, "A" );

  pthread_join( p1, NULL );

  pthread_create( &p2, NULL, mythread, "B" );

  pthread_join( p2, NULL );

  pthread_create( &p3, NULL, mythread, "C" );

  pthread_join( p3, NULL );

  printf( "Final Balance is %d\n", balance );
}