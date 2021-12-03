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
  // const char * inFile  = "inFile.txt";
  // const char * outFile = "outFile.txt";
  const char * inFile  = argv[1];
  const char * outFile = argv[2];

  int         pageSize = getpagesize();
  struct stat srcStat;
  int         srcFD = open( inFile, O_RDONLY );
  int         dstFD = open( outFile, O_RDWR | O_CREAT, 0700 );

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
    return 1;
  }
  ftruncate( dstFD, srcStat.st_size );
  for( int i = 0; i < srcStat.st_size; i += pageSize )
  {
    if( srcStat.st_size - i < pageSize )
    {
      pageSize = srcStat.st_size - i;
      cout << "pageSize: " << pageSize << endl;
    }
    // cout << "Mapping " << i << " bytes" << endl;
    char * srcData = (char *) mmap( NULL, pageSize, PROT_READ, MAP_SHARED, srcFD, i );
    char * dstData = (char *) mmap( NULL, pageSize, PROT_WRITE, MAP_SHARED, dstFD, i );
    if( srcData == nullptr or dstData == nullptr )
    {
      cout << "Error mapping files" << endl;
      exit( 1 );
    }
    memcpy( dstData, srcData, pageSize );
    munmap( srcData, srcStat.st_size );
    munmap( dstData, srcStat.st_size );
  }


  /* Close the file */
  close( srcFD );
  close( dstFD );

  return 0;
}
