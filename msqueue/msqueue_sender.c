#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
   * If the message queue does not exist, then
   * create it.
   */
  int msqid = msgget( key, 0666 | IPC_CREAT );

  /* Make sure the queue was successfully creted */
  if( msqid < 0 )
  {
    perror( "msgget" );
    exit( -1 );
  }

  /* The message.  */
  struct msgBuff msg;

  /* Set the message type to 2. Any positive value will do */
  msg.mtype = 2;

  /* Set the payload to "Hello World" string */
  strncpy( msg.data, "Hello world", 12 );

  /* Send the message
   * @param msqid - the queue into which the message will be placed
   * @param msg - the actual message
   * @param sizeof(msgBuff) - sizeof(long): the size of the message payload
   * i.e. data not counting the required long mtype.
   * @param 0 - the miscellenous flags.
   */
  if( msgsnd( msqid, &msg, sizeof( msg ) - sizeof( long ), 0 ) < 0 )
  {
    perror( "msgsnd" );
    exit( -1 );
  }


  return 0;
}
