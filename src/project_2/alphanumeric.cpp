#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <vector>

std::string              str;
std::vector<std::string> splitStr;
int                      curIndex = 0;
bool                     lock     = false;

std::vector<std::string> split( std::string str, char delim = ' ' );
void *                   alpha( void * param );
void *                   numeric( void * param );

int main( int argc, char * argv[] )
{
  pthread_t p1 = 0;
  pthread_t p2 = 0;

  if( argc != 2 )
  {
    fprintf( stderr, "usage: %s <string>\n", argv[0] );
    return -1;
  }

  str      = argv[1];
  splitStr = split( str );

  pthread_create( &p1, nullptr, alpha, nullptr );
  pthread_create( &p2, nullptr, numeric, nullptr );

  pthread_join( p1, nullptr );
  pthread_join( p2, nullptr );
}

std::vector<std::string> split( std::string str, char delim )
{
  std::vector<std::string> split_str;
  std::string              word;
  unsigned int             i = 0;
  while( i < str.length() )
  {
    if( str[i] == delim )
    {
      split_str.push_back( word );
      word = "";
    }
    else
    {
      word += str[i];
    }
    i++;
  }
  split_str.push_back( word );
  return split_str;
}

void * alpha( void * /*param*/ )
{
  while( curIndex < splitStr.size() )
  {
    while( lock ) {}
    if( curIndex < splitStr.size() && isalpha( splitStr[curIndex][0] ) != 0 )
    {
      lock = true;
      fprintf( stdout, "alpha: %s\n", splitStr[curIndex].c_str() );
      curIndex++;
      lock = false;
    }
  }
  pthread_exit( nullptr );
}

void * numeric( void * /*param*/ )
{
  while( curIndex < splitStr.size() )
  {
    while( lock ) {}
    if( curIndex < splitStr.size() && isdigit( splitStr[curIndex][0] ) != 0 )
    {
      lock = true;
      fprintf( stdout, "numeric: %s\n", splitStr[curIndex].c_str() );
      curIndex++;
      lock = false;
    }
  }
  pthread_exit( nullptr );
}