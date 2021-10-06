#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
  int   i = 0;
  pid_t id = 0;
  id = fork();
  fprintf( stderr, "This is process %d \n", getpid() );
  fprintf( stderr, "id %d\n", id );
  printf( "Hello\n" );

  wait( NULL );
  id = fork();
  fprintf( stderr, "This is process %d \n", getpid() );
  fprintf( stderr, "id %d\n", id );
  printf( "Hello\n" );

  wait( NULL );

  return 0;
}
