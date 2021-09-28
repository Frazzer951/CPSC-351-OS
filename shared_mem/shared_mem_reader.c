#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main( int argc, char ** argv )
{
  /* A string buffer */
  char strBuff[1024];

  /* Fill the buffer with 0's */
  memset( strBuff, 0, 1024 );

  /* Find a segment with key 1235, of size 1024 bytes, and the region is
   * readable, writable, and executable (if the segment with key 1235 does
   * not exit, then exit with an error
   */
  int segment_id = shmget( 1235, 1024, IPC_EXCL | S_IRUSR | S_IWUSR );

  /* Failed to find shared memory */
  if( segment_id < 0 )
  {
    perror( "shmget" );
    exit( -1 );
  }

  /* Get a pointer to the shared memory  */
  char * shared_memory = (char *) shmat( segment_id, 0, 0 );

  /*  Error checks */
  if( !( (void *) shared_memory ) )
  {
    perror( "shmat" );
    exit( -1 );
  }

  /* Copy the first 12 bytes of memory to the local buffer */
  strncpy( strBuff, shared_memory, 12 );

  /* Print what we just read */
  fprintf( stderr, "%s\n", strBuff );

  /* Deallocate the memory segment */
  if( shmctl( segment_id, IPC_RMID, 0 ) < 0 ) { perror( "shmctl" ); }

  return 0;
}
