#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
  int i;
  pid_t  id;
  id = fork();
  fprintf(stderr, "This is process %d \n", getpid());
  fprintf(stderr, "id %d\n", id);
  printf("Hello\n");

  wait(NULL);
  id = fork();
  fprintf(stderr, "This is process %d \n", getpid());
  fprintf(stderr, "id %d\n", id);
  printf("Hello\n");

  wait(NULL);

  return 0;
}
