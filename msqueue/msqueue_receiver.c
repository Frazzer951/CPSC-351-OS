#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>

/* Message Buffer */
struct msgBuff
{
  /* All message buffers must start with this long.
   * It's used by the receiver for message selection.
   */
  long mtype;

  /* The actual data */
  char data[100];
};


int main()
{
  /* Use some random file path and a random
   * character in order to generate a unique
   * key to be used with memory.
   */
  key_t key = ftok( "/bin/ls", 'b' );

  /* Make sure the key generation succeeded */
  if( key < 0 )
  {
    perror( "ftok" );
    exit( -1 );
  }


  /* Associated a message queue with the key.
   * DO NOT create a new queue if it does not
   * exist.
   */
  int msqid = msgget( key, 0666 );

  /* Make sure the queue was successfully creted */
  if( msqid < 0 )
  {
    perror( "msgget" );
    exit( -1 );
  }

  /* The buffer into which to store the received message */
  struct msgBuff msg;

  /* Pull a message out of a queue
   * @param msqid - the queue from which to fetch the message
   * @param msg - the buffer in which the retreived message will be stored.
   * @param sizeof(msgBuff) - sizeof(long): the size of the message payload
   * i.e. data not counting the required long mtype.
   * @param 2 - the type of message to receive; this specifies to receive
   * all messages whose mtype is 2 (this is what mtype was set to in the sender).
   * @param 0 - the miscellenous flags.
   */
  if( msgrcv( msqid, &msg, sizeof( msg ) - sizeof( long ), 2, 0 ) < 0 )
  {
    perror( "msgsnd" );
    exit( -1 );
  }

  /* Print the payload */
  fprintf( stderr, "%s\n", msg.data );

  return 0;
}
