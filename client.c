#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define PORT_NO 8001

void cliepro(int);

int main(int argc, char **argv)
{

  int sofd;                   
  struct hostent *shost;      
  struct sockaddr_in sv_addr; 

sofd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     


    shost = gethostbyname("10.0.0.20");
      if (shost == NULL) {
      exit(EXIT_FAILURE);
}


  


  bzero((void *)&sv_addr,sizeof(sv_addr));
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(PORT_NO);
  memcpy((void *)&sv_addr.sin_addr,(void *)shost->h_addr,shost->h_length);
  
  if (connect(sofd, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
    perror("yes");
      exit(EXIT_FAILURE);
}
  

  cliepro(sofd);
  
  close(sofd);

  exit(0);
}

void cliepro(int sofd)
{
  int cc,nbyte, MAXRMSG;
  char smsg[100], rmsg[100];
  
  MAXRMSG=sizeof(rmsg);
  bzero(rmsg, MAXRMSG);
  
  while(1){
    printf("Enter string :");
    fgets(smsg, sizeof(smsg), stdin);
    if(feof(stdin)) break;
    nbyte=strlen(smsg);

    if (send(sofd, smsg, nbyte, 0) < 0) {
      perror("send");
    } else {
      cc=recv(sofd,rmsg,MAXRMSG,0);
      if(cc<0) perror("recv");
      else {
        rmsg[cc]='\0';
	printf("%s\n",rmsg);
	bzero(smsg,MAXRMSG);
      }
    }

  }
	
}
