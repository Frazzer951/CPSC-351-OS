
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


/**
 * The function to be executed by the child
 */
void child()
{

	/** NOTE: getppid() returns the process id of the parent of the caller process **/
	fprintf(stderr, "Hi! I am the child. My parent pid is %d \n", getppid());
		
	/** NOTE: getpid() returns the process id of the caller process **/
	fprintf(stderr, "Hi! I am the child. My pid is %d. I am done now...\n", getpid());
	
		
}

int main(int argc, char** argv)
{
	/* The process id of the child */
	pid_t child_pid;
	
	/* If the fork failed, then terminate. Otherwise, save the  
	 * value returned by fork(). 
	 */
	if((child_pid = fork()) < 0)
	{
		perror("fork");
		exit(-1);
	}
	
	/* Are we a child process ? */
	if(child_pid == 0)
	{
		/* Call the child function */
		child();
	}
	/* We are the parent */
	else
	{
		fprintf(stderr, "Hi! I am a parent. Just created a child with pid %d\n", child_pid);
		
		/* The parent loops infintely; wait will never be called */
		for(;;);
			
		/* Wait for the child to exit */
		if(wait(NULL) < 0)
		{
			perror("wait");
			exit(-1);
		}	
	
	}	
	return 0;
}
