#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <vector>

std::vector<int> fib;

void * fibonacci( void * param );

int main( int argc, char * argv[] )
{
  pthread_t p1 = 0;

  if( argc != 2 )
  {
    fprintf( stderr, "usage: %s <integer value>\n", argv[0] );
    return -1;
  }

  if( isdigit( argv[1][0] ) == 0 )
  {
    fprintf( stderr, "%s is not a valid integer\n", argv[1] );
    return -1;
  }

  pthread_create( &p1, nullptr, fibonacci, argv[1] );

  pthread_join( p1, nullptr );

  printf( "Fibonacci sequence: " );

  for( int i : fib ) { fprintf( stdout, "%d ", i ); }

  printf( "\n" );
}

void * fibonacci( void * param )
{
  int fib_count = atoi( (char *) param );

  int i = 0;
  while( i < fib_count )
  {
    if( i == 0 ) fib.push_back( 0 );
    else if( i == 1 )
      fib.push_back( 1 );
    else
      fib.push_back( fib[i - 1] + fib[i - 2] );
    i++;
  }

  pthread_exit( nullptr );
}