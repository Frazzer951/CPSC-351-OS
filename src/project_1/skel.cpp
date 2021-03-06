#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

/* The pipe for parent-to-child communications */
int parentToChildPipe[2];

/* The pipe for the child-to-parent communication */
int childToParentPipe[2];

/* The read end of the pipe */
#define READ_END 0

/* The write end of the pipe */
#define WRITE_END 1

/* The maximum size of the array of hash programs */
#define HASH_PROG_ARRAY_SIZE 6

/* The maximum length of the hash value */
#define HASH_VALUE_LENGTH 1000

/* The maximum length of the file name */
#define MAX_FILE_NAME_LENGTH 1000

/* The array of names of hash programs */
const string hashProgs[] = { "md5sum", "sha1sum", "sha224sum", "sha256sum", "sha384sum", "sha512sum" };

/**
 * The function called by a child
 * @param hashProgName - the name of the hash program
 */
void computeHash( const string & hashProgName )
{
  /* The hash value buffer */
  char hashValue[HASH_VALUE_LENGTH];

  /* The received file name string */
  char fileNameRecv[MAX_FILE_NAME_LENGTH];

  /* Fill the buffer with 0's */
  memset( fileNameRecv, (char) NULL, MAX_FILE_NAME_LENGTH );

  /* Read message from the parent */
  if( read( parentToChildPipe[READ_END], fileNameRecv, sizeof( fileNameRecv ) ) < 0 )
  {
    perror( "read" );
    exit( -1 );
  }

  /* Close the pipe */
  if( close( parentToChildPipe[READ_END] ) < 0 )
  {
    perror( "close" );
    exit( -1 );
  }

  /* Glue together a command line <PROGRAM NAME>.
   * For example, sha512sum fileName.
   */
  string cmdLine( hashProgName );
  cmdLine += " ";
  cmdLine += fileNameRecv;

  /* Call Popen with cmdLine */
  FILE * progOutput = popen( cmdLine.c_str(), "r" );

  if( progOutput == nullptr )
  {
    perror( "popen" );
    exit( -1 );
  }

  /* Reset the value buffer */
  memset( hashValue, (char) NULL, HASH_VALUE_LENGTH );

  if( fread( hashValue, sizeof( char ), sizeof( char ) * HASH_VALUE_LENGTH, progOutput ) < 0 )
  {
    perror( "fread" );
    exit( -1 );
  }

  /* Send a string to the parent */
  if( write( childToParentPipe[WRITE_END], hashValue, sizeof( hashValue ) ) < 0 )
  {
    perror( "write" );
    exit( -1 );
  }

  /* Close the Pipe */
  if( close( childToParentPipe[WRITE_END] ) < 0 )
  {
    perror( "close" );
    exit( -1 );
  }

  /* The child terminates */
  exit( 0 );
}

int main( int argc, char ** argv )
{
  /* Check for errors */
  if( argc < 2 )
  {
    fprintf( stderr, "USAGE: %s <FILE NAME>\n", argv[0] );
    exit( -1 );
  }

  /* Save the name of the file */
  string fileName( argv[1] );

  /* The process id */
  pid_t pid = 0;

  /* Run a program for each type of hashing algorithm hash algorithm */
  for( const auto & hashProg : hashProgs )
  {
    /* Create a parent-to-child pipe */
    if( pipe( parentToChildPipe ) < 0 )
    {
      perror( "pipe" );
      exit( -1 );
    }

    /* Create a child-to-parent pipe */
    if( pipe( childToParentPipe ) < 0 )
    {
      perror( "pipe" );
      exit( -1 );
    }

    /* Fork a child process and save the id */
    if( ( pid = fork() ) < 0 )
    {
      perror( "fork" );
      exit( -1 );
    }
    /* I am a child */
    else if( pid == 0 )

    {
      /* Close the write-end of the parent-to-child pipe */
      if( close( parentToChildPipe[WRITE_END] ) < 0 )
      {
        perror( "close" );
        exit( -1 );
      }

      /* Close the read-end of the child-to-parent pipe */
      if( close( childToParentPipe[READ_END] ) < 0 )
      {
        perror( "close" );
        exit( -1 );
      }

      /* Compute the hash */
      computeHash( hashProg );
    }

    /* I am the parent */

    /* Close the read-end of the parent-to-child pipe */
    if( close( parentToChildPipe[READ_END] ) < 0 )
    {
      perror( "close" );
      exit( -1 );
    }

    /* Close the write-end of the child-to-parent pipe */
    if( close( childToParentPipe[WRITE_END] ) < 0 )
    {
      perror( "close" );
      exit( -1 );
    }

    /* The buffer to hold the string received from the child */
    char hashValue[HASH_VALUE_LENGTH];

    /* Reset the hash buffer */
    memset( hashValue, (char) NULL, HASH_VALUE_LENGTH );

    /* Send a string to the child */
    if( write( parentToChildPipe[WRITE_END], fileName.c_str(), sizeof( fileName.c_str() ) ) < 0 )
    {
      perror( "write" );
      exit( -1 );
    }

    /* Close the Pipe */
    if( close( parentToChildPipe[WRITE_END] ) < 0 )
    {
      perror( "close" );
      exit( -1 );
    }

    /* TODO: Read the string sent by the child
     .
     .
     .
     */
    /* Read message from the child */
    if( read( childToParentPipe[READ_END], hashValue, sizeof( hashValue ) ) < 0 )
    {
      perror( "read" );
      exit( -1 );
    }

    /* Close the pipe */
    if( close( childToParentPipe[READ_END] ) < 0 )
    {
      perror( "close" );
      exit( -1 );
    }

    /* Print the hash value */
    fprintf( stdout, "%s HASH VALUE: %s\n", hashProg.c_str(), hashValue );
    fflush( stdout );


    /* Wait for the program to terminate */
    if( wait( nullptr ) < 0 )
    {
      perror( "wait" );
      exit( -1 );
    }
  }

  return 0;
}
