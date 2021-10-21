#include <pthread.h>
#include <stdio.h>

int             count = 0;
pthread_t       t1, t2;
pthread_mutex_t myMutex;

void * runner( void * param )
{
  pthread_mutex_lock( &myMutex );
  for( int i = 0; i < 1000000; ++i ) count += 1;

  fprintf( stderr, "count = %d\n", count );
  pthread_mutex_unlock( &myMutex );
  pthread_exit( 0 );
}

int main( int argc, char * argv[] )
{
  pthread_attr_t attr; /* Set of thread attributes */

  /* Get the default attributes */
  pthread_attr_init( &attr );

  /* Initialize the mutex */
  pthread_mutex_init( &myMutex, NULL );

  /* create the thread */
  pthread_create( &t1, &attr, runner, NULL );
  pthread_create( &t2, &attr, runner, NULL );
  /* wait for the thread to exit */
  pthread_join( t1, NULL );
  pthread_join( t2, NULL );
}    // End of main()
