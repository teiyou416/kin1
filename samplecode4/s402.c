#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
int x;

void catch_sigchild(int sig)
{
  pid_t c_pid;
  int status;

  if ((c_pid = wait(&status)) == -1) {
    perror("wait");
    return;
  }
  printf("Child(%d) Terminate status (%d)\n", c_pid, status);

}

int main(void)
{
  pid_t pid, status;
  int y;
  int rc;
  char buf[10];

  if (signal(SIGCHLD, catch_sigchild) != 0) {
    perror("Signal");
    exit(-1);
  }

  pid=fork();

  if (pid==0) {
    x=5;
    y=5;
    printf("I am Child Process: x=%d y=%d\n", x, y);
  } else {
    x=10;
    y=10;
    printf("I am Parent Process for Child(%d): x=%d y=%d\n", pid, x, y);
    fgets(buf, sizeof(buf), stdin);
  }
  exit(2);
}
