#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main( int argc, char ** argv )

{
  int   a         = 0;
  pid_t child_pid = 0;

  child_pid = fork();

  a++;

  if( child_pid == 0 )
  {
    pid_t grand_child_pid = 0;
    grand_child_pid       = fork();
    printf( "Here ?\n" );
    a++;
    printf( "a Here ? %d\n", a );
  }
  else
  {
    a--;
  }

  printf( "Hello!\n" );
  fprintf( stderr, "This is process %d \n", getpid() );
  printf( "a is %d\n", a );

  wait( NULL );

  return 0;
}
