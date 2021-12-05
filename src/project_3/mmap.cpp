#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main( int argc, char ** argv )
{
  if( argc != 3 )
  {
    cout << "Usage: " << argv[0] << " <inFile> <outFile>" << endl;
    return 1;
  }

  const char * inFile  = argv[1];
  const char * outFile = argv[2];

  struct stat srcStat;
  int         pageSize = getpagesize();
  int         srcFD    = open( inFile, O_RDONLY );
  int         dstFD    = open( outFile, O_RDWR | O_CREAT | O_TRUNC, 0700 );

  if( srcFD < 0 )
  {
    cout << "Error opening input file: " << inFile << endl;
    exit( 1 );
  }
  if( stat( inFile, &srcStat ) < 0 )
  {
    cout << "Error getting file size: " << inFile << endl;
    exit( 1 );
  }
  if( dstFD < 0 )
  {
    cout << "Error creating output file: " << outFile << endl;
    exit( 1 );
  }
  if( ftruncate( dstFD, srcStat.st_size ) < 0 )
  {
    cout << "Error Resizing output file: " << outFile << endl;
    exit( 1 );
  }

  for( int i = 0; i < srcStat.st_size; i += pageSize )
  {
    char * srcData = (char *) mmap( NULL, pageSize, PROT_READ, MAP_SHARED, srcFD, i );
    char * dstData = (char *) mmap( NULL, pageSize, PROT_WRITE, MAP_SHARED, dstFD, i );
    if( !srcData || !dstData )
    {
      cout << "Error mapping files" << endl;
      exit( 1 );
    }
    memcpy( dstData, srcData, pageSize );
    munmap( srcData, pageSize );
    munmap( dstData, pageSize );
  }


  /* Close the file */
  close( srcFD );
  close( dstFD );

  return 0;
}
