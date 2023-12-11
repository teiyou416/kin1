#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc,char **argv)
{
  struct hostent *shost;
  char *addr;
  struct in_addr in;

  if (argc < 2) {
    printf("Usage : %s hostname\n", argv[0]);
    exit(1);
  }

  shost = gethostbyname(argv[1]);
  if(shost==NULL) {
    perror("gethostbyname");
    exit(-1);
  }

  in.s_addr = *((in_addr_t *)(shost->h_addr_list[0]));
  addr = inet_ntoa(in);
  printf("IP address of %s is %s\n", argv[1], addr);
}

