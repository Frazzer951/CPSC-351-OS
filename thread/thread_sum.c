#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int    sum;                    /* this data is shared by the thread(s) */
void * runner( void * param ); /* the thread */

int main( int argc, char * argv[] )
{
  pthread_t      tid = 0;  /* the thread identifier */
  pthread_attr_t attr; /* set of thread attributes */

  fprintf( stderr, "process id %d \n", getpid() );
  if( argc != 2 )
  {
    fprintf( stderr, "usage: a.out <integer value>\n" );
    return -1;
  }

  if( atoi( argv[1] ) < 0 )
  {
    fprintf( stderr, "%d must be >= 0\n", atoi( argv[1] ) );
    return -1;
  }
  /* get the default attributes */
  pthread_attr_init( &attr );

  /* create the thread */
  pthread_create( &tid, &attr, runner, argv[1] );
  fprintf( stderr, "created the thread with the id %lu \n", tid );
  /* wait for the thread to exit */
  pthread_join( tid, NULL );

  printf( "sum = %d\n", sum );
}    // End of main()

/* The thread will begin control in this function */
void * runner( void * param )
{
  int i;
  int upper = atoi( param );
  sum = 0;
  fprintf( stderr, "process id %d \n", getpid() );
  for( i = 1; i <= upper; i++ ) sum += i;

  pthread_exit( 0 );
}
