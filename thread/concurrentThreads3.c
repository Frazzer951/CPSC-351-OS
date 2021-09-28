#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int j = 0;

void * thread_func( void * arg )
{
  for( int i = 0; i < 1000000; i++ )
  {
    j++;

    /*char* message;
    message = (char*)arg;
    sleep(1);
    printf("%s \n", "*********");
    printf("%s \n", message);*/
  }

  printf( "j is %d\n", j );

  pthread_exit( 0 );
}

int main( int argc, char * argv[] )
{
  pthread_t p1, p2;

  pthread_create( &p1, NULL, thread_func, "thread1" );
  pthread_create( &p2, NULL, thread_func, "thread2" );

  pthread_join( p1, NULL );
  pthread_join( p2, NULL );
}
