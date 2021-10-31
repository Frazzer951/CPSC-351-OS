//
//  prodcons.c
//  producer_consumer
//
//  Created by William McCarthy on 12/10/2021.
//

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
//#include <sys/semaphore.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 10    // Choice of size > 5; size = 10 == Size of the buffer.
#define WRAPS       3

#define PRODUCERS 1    // Multi producer = 1
#define CONSUMERS 2    // Multi consumer = 1

#define MAX_PRODUCERS 2    // Max = 2     Max items a producer can produce
#define MAX_CONSUMERS 2    // Max - 2     Max items a consumer can consume

int array[BUFFER_SIZE];
int producer_idx = 0;
int consumer_idx = 0;
int begin        = 10;
int filled_      = 0;
int empty_       = BUFFER_SIZE;

pthread_mutex_t mutex_;

sem_t * max_producers;
sem_t * max_consumers;
sem_t * deliveries_OK;
sem_t * HAS_deliveries;

int  filled( int * array ) { return filled_; }
int  empty( int * array ) { return BUFFER_SIZE - filled( array ); }
bool is_filled( int * array ) { return empty( array ) == 0; }
bool is_empty( int * array ) { return filled( array ) == 0; }

// Show - print out all the numbers, and prints out all the values. Tells you what's filled and what's empty.
void show( int * array )
{
  for( int i = 0; i < 10; ++i ) { printf( "%2d ", i ); }
  printf( "\n" );
  for( int i = 0; i < 10; ++i ) { printf( "%2d ", array[i] ); }
  printf( "\t\t\tFILLED IS: %d,  EMPTY is %d\n\n", filled( array ), empty( array ) );
}

// Produce
void produce( int id, int * counter )
{
  pthread_mutex_lock( &mutex_ );
  printf( "\t\t\tSET %d in index %d (prod# %d)\n", begin, producer_idx, id );
  array[producer_idx++] = begin++;
  producer_idx %= BUFFER_SIZE;
  ++*counter;
  ++filled_;
  show( array );
  pthread_mutex_unlock( &mutex_ );
}

// Producer Runner
void * producer_runner( void * param )
{
  int counter = 0;
  int id      = (int) (long) param;

  pthread_mutex_lock( &mutex_ );
  printf( "\n\n..................................producer thread %d starting\n\n", id );
  pthread_mutex_unlock( &mutex_ );

  while( counter < WRAPS * BUFFER_SIZE )
  {
    sem_wait( max_producers );
    if( is_filled( array ) )
    {
      printf( "\nbuffer is full:  calling sem_wait on deliveries_OK\n" );
      sem_wait( deliveries_OK );
    }
    printf( "\nProducer %d through semaphore, waiting on mutex...\n", id );

    produce( id, &counter );

    if( filled( array ) == 1 )
    {
      printf( "\nNow has 1 delivery, calling sem_post on HAS_deliveries\n" );
      sem_post( HAS_deliveries );
    }    // pthread_mutex_unlock(&mutex_);
    sem_post( max_producers );
  }
  pthread_mutex_lock( &mutex_ );
  printf( "\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>> producer thread %d terminating\n\n", id );
  pthread_mutex_unlock( &mutex_ );

  pthread_exit( NULL );
}

// Consume - get a value, lock the mutex.
void consume( int id, int * counter )
{
  pthread_mutex_lock( &mutex_ );

  int val = array[consumer_idx];
  if( val == -1 )
  {
    perror( "invalid GOT in consume" );
  }    // If value is -1, do perror. (Must always do positive values)
  printf( "\n\t\t\t\t\tGOT %d from index %d (cons# %d)\n", array[consumer_idx], consumer_idx, id );    // Print value
  array[consumer_idx++] = -1;    // After its finished, put -1 into the buffer after we've gone throught it. -2 = gone
                                 // over two times, -3 = gone over three times
  consumer_idx %= BUFFER_SIZE;
  ++*counter;
  --filled_;
  show( array );

  pthread_mutex_unlock( &mutex_ );
}

// Consumer Runner
void * consumer_runner( void * param )
{
  int counter = 0;
  int id      = (int) (long) param;

  pthread_mutex_lock( &mutex_ );    // Set a mutex lock
  printf( "\n\n..................................consumer thread %d starting\n\n", id );
  pthread_mutex_unlock( &mutex_ );

  while( counter < WRAPS * BUFFER_SIZE )
  {
    sem_wait( max_consumers );

    // If buffer is empty, trigger sem_wait until there's delivers.
    if( is_empty( array ) )
    {
      printf( "\nBuffer is empty -- calling sem_wait(HAS_deliveries);\n" );
      sem_wait( HAS_deliveries );
    }

    // Once its signal to go ahead,
    printf( "\nConsumer %d through semaphore, waiting on mutex...\n", id );
    consume( id, &counter );

    // Buffer has items but not full, say nothing.
    if( filled( array ) == BUFFER_SIZE - 1 )
    {
      printf( "\nbuffer now at size-1, calling sem_post on deliveries_OK...\n" );
      sem_post( deliveries_OK );
    }

    sem_post( max_consumers );
  }

  pthread_mutex_lock( &mutex_ );
  printf( "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>> consumer thread %d terminating\n", id );
  pthread_mutex_unlock( &mutex_ );

  pthread_exit( NULL );
}

// Create Semaphore
sem_t * create_semaphore( const char * sem_name, int pshared, unsigned int value )
{
  sem_t * semaphore = NULL;
  if( ( semaphore = sem_open( sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, value ) ) == SEM_FAILED )
  {
    perror( "sem_open failed -- trying again" );

    sem_unlink( sem_name );
    if( ( semaphore = sem_open( sem_name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, value ) ) == SEM_FAILED )
    {
      perror( "sem_open failed -- terminating" );
      exit( -1 );
    }
  }
  //  sem_init(semaphore, pshared, value);
  printf( "created semaphore: \'%s\'... \n\n", sem_name );
  return semaphore;
}

// typedef struct sem sem;
// struct sem {
//   const char* name;
//   sem_t* semaphore;
//
// };
//
// sem one;
// one.name = "my name";
//
// void remove_semaphore(sem* semaphore) {
//   sem_close(semaphore.sem);
//   sem_unlink(semaphore.name);
// }

// Remove Semaphore ... Semaphore CLOSE AND UNLINK SEMAPHORE
void remove_semaphore( const char * sem_name, sem_t * semaphore )
{
  sem_close( semaphore );
  sem_unlink( sem_name );
  //  sem_destroy(semaphore);
}

// Main
int main( int argc, const char * argv[] )
{
  pthread_t      producers[PRODUCERS];
  pthread_t      consumers[CONSUMERS];
  const char *   max_consumersname  = "max_consumers";
  const char *   max_producersname  = "max_producers";
  const char *   deliveries_OKname  = "deliveries_OK";     // Empty buffer
  const char *   HAS_deliveriesname = "HAS_deliveries";    // Full buffer
  pthread_attr_t attr;

  //
  max_producers  = create_semaphore( max_producersname, 1, MAX_PRODUCERS );
  max_consumers  = create_semaphore( max_consumersname, 1, MAX_CONSUMERS );
  deliveries_OK  = create_semaphore( deliveries_OKname, 1, 1 );
  HAS_deliveries = create_semaphore( HAS_deliveriesname, 1, 0 );

  // Create a thread for consumer
  pthread_attr_init( &attr );
  for( int i = 0; i < CONSUMERS; ++i ) { pthread_create( &consumers[i], &attr, consumer_runner, (void *) (long) i ); }

  // Create a thread for Producer
  for( int i = 0; i < PRODUCERS; ++i ) { pthread_create( &producers[i], &attr, producer_runner, (void *) (long) i ); }

  // Join the threads
  for( int i = 0; i < PRODUCERS; ++i ) { pthread_join( producers[i], NULL ); }
  for( int i = 0; i < CONSUMERS; ++i ) { pthread_join( consumers[i], NULL ); }

  // Create multiple producers/one consumer


  // Multiple producers and consumers

  remove_semaphore( max_consumersname, max_consumers );
  remove_semaphore( max_producersname, max_producers );
  remove_semaphore( HAS_deliveriesname, HAS_deliveries );
  remove_semaphore( deliveries_OKname, deliveries_OK );
}

// SKELETON CODE --- IT IS NOT COMPLETE --- YOU HAVE TO WORK WITH THE SEMAPHORES
//   AND THE READ WRITE CODE TO GET IT TO WORK
//   (1) ONE PRODUCER/ONE CONSUMER
//   (2) MULTIPLE PRODUCERS/ONE CONSUMER
//   (3) MULTIPLE PRODUCERS and CONSUMERS

// created semaphore: 'max_producers'...
//
// created semaphore: 'max_consumers'...
//
// created semaphore: 'deliveries_OK'...
//
// created semaphore: 'HAS_deliveries'...
//
//
//
//..................................consumer thread 0 starting
//
//
//
//..................................producer thread 0 starting
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 10 in index 0 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 10  0  0  0  0  0  0  0  0  0       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 11 in index 1 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 10 11  0  0  0  0  0  0  0  0       FILLED IS: 2,  EMPTY is 8
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 12 in index 2 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 10 11 12  0
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 10 from index 0 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 11 12  0  0  0  0  0  0  0       FILLED IS: 2,  EMPTY is 8
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 11 from index 1 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 12  0  0  0  0  0  0  0       FILLED IS: 1,  EMPTY is 9
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 12 from index 2 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1  0  0  0  0  0  0  0       FILLED IS: 0,  EMPTY is 10
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//  0  0  0  0  0  0       FILLED IS: 0,  EMPTY is 10
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 13 in index 3 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 13  0  0  0  0  0  0       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 14 in index 4 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 13 14  0  0  0  0  0       FILLED IS: 2,  EMPTY is 8
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 15 in index 5 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 13 14 15  0  0  0  0       FILLED IS: 3,  EMPTY is 7
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 16 in index 6 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 13 14 15 16  0  0  0       FILLED IS: 4,  EMPTY is 6
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 13 from index 3 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 14 15 16  0  0  0       FILLED IS: 3,  EMPTY is 7
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 17 in index 7 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 14 15 16 17  0  0       FILLED IS: 4,  EMPTY is 6
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 14 from index 4 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 15 16 17  0  0       FILLED IS: 3,  EMPTY is 7
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 18 in index 8 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 15 16
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 15 from index 5 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 16 17 18  0       FILLED IS: 3,  EMPTY is 7
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 16 from index 6 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 17 18  0       FILLED IS: 2,  EMPTY is 8
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 17 from index 7 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 18  0       FILLED IS: 1,  EMPTY is 9
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 18 from index 8 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1  0       FILLED IS: 0,  EMPTY is 10
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
// 17 -1  0       FILLED IS: 0,  EMPTY is 10
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 19 in index 9 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 19       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 20 in index 0 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 20 -1 -1 -1 -1 -1 -1 -1 -1 19       FILLED IS: 2,  EMPTY is 8
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 21 in index 1 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 20 21 -1 -1 -1 -1 -1 -1 -1 19       FILLED IS: 3,  EMPTY is 7
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 19 from index 9 (cons# 0)
//  0  1  2  3  4  5
// Producer 0 through semaphore, waiting on mutex...
//       SET 22 in index 2 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 20 21 22 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 3,  EMPTY is 7
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 23 in index 3 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 20 21 22 23 -1 -1 -1 -1 -1 -1       FILLED IS: 4,  EMPTY is 6
//
//  6  7  8  9
// 20 21 22 23 -1 -1 -1 -1 -1 -1       FILLED IS: 4,  EMPTY is 6
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 20 from index 0 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 21 22 23 -1 -1 -1 -1 -1 -1       FILLED IS: 3,  EMPTY is 7
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 21 from index 1 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 22 23 -1 -1 -1 -1 -1 -1       FILLED IS: 2,  EMPTY is 8
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 22 from index 2 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 23 -1 -1 -1 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 23 from index 3 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 24 in index 4 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 24 -1 -1 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 24 from index 4 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 25 in index 5 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 25 -1 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 26 in index 6 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 25 26 -1 -1 -1       FILLED IS: 2,  EMPTY is 8
//
//
// Producer 0 through semaphore, waiting on mutex...
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 25 from index 5 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 26 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 26 from index 6 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//       SET 27 in index 7 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 27 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 28 in index 8 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 27 28 -1       FILLED IS: 2,  EMPTY is 8
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 27 from index 7 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 28 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 29 in index 9 (prod# 0)
//  0  1  2  3  4
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 28 from index 8 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 29       FILLED IS: 1,  EMPTY is 9
//
//  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 29       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 29 from index 9 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 30 in index 0 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
// 30 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 30 from index 0 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 31 in index 1 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 31 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 31 from index 1 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//
// Consumer 0 through semaphore, waiting on mutex...
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 32 in index 2 (prod# 0)
//  0 invalid GOT in consume: Undefined error: 0
//  1  2  3  4  5  6  7  8  9
//-1 -1 32 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
//           GOT 32 from index 2 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 33 in index 3 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 33 -1 -1 -1 -1 -1 -1       FILLED IS: 1,  EMPTY is 9
//
//
// Now has 1 delivery, calling sem_post on HAS_deliveries
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 34 in index 4 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 33 34 -1 -1 -1 -1 -1       FILLED IS: 2,  EMPTY is 8
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 35 in index 5 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 33 34 35 -1 -1 -1 -1       FILLED IS: 3,  EMPTY is 7
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 36 in index 6 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 33 34 35 36 -1 -1 -1       FILLED IS: 4,  EMPTY is 6
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 37 in index 7 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 33 34 35 36 37 -1 -1       FILLED IS: 5,  EMPTY is 5
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 38 in index 8 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 33 34 35 36 37 38 -1       FILLED IS: 6,  EMPTY is 4
//
//
// Producer 0 through semaphore, waiting on mutex...
//       SET 39 in index 9 (prod# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 33 34 35 36 37 38 39       FILLED IS: 7,  EMPTY is 3
//
//
//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>> producer thread 0 terminating
//
//
// Buffer is empty -- calling sem_wait(HAS_deliveries);
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 33 from index 3 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 34 35 36 37 38 39       FILLED IS: 6,  EMPTY is 4
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 34 from index 4 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 35 36 37 38 39       FILLED IS: 5,  EMPTY is 5
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 35 from index 5 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 36 37 38 39       FILLED IS: 4,  EMPTY is 6
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 36 from index 6 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 37 38 39       FILLED IS: 3,  EMPTY is 7
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 37 from index 7 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 38 39       FILLED IS: 2,  EMPTY is 8
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 38 from index 8 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 39       FILLED IS: 1,  EMPTY is 9
//
//
// Consumer 0 through semaphore, waiting on mutex...
//
//           GOT 39 from index 9 (cons# 0)
//  0  1  2  3  4  5  6  7  8  9
//-1 -1 -1 -1 -1 -1 -1 -1 -1 -1       FILLED IS: 0,  EMPTY is 10
//
//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>> consumer thread 0 terminating
// Program ended with exit code: 0
