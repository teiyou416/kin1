#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int x;

int main(void)
{
  pid_t pid, status;
  int y;
  char buf[100];

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
    pid=wait(&status);
    fgets(buf, sizeof(buf), stdin);
    printf("Child(%d) terminated: exit-code = %d\n",pid, WEXITSTATUS(status));
  }
  exit(2);
}
