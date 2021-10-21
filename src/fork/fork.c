
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
  pid_t pid = 0;
  /* fork another process */
  pid = fork();
  if( pid < 0 )
  { /* error occurred */
    fprintf( stderr, "Fork Failed" );
    exit( -1 );
  }
  else if( pid == 0 )
  { /* child process */
    execlp( "/bin/ls", "ls", NULL );
  }
  else
  { /* parent process */
    /* parent will wait for the child to complete */
    wait( NULL );
    printf( "\n Child Complete \n" );
    exit( 0 );
  }
}
