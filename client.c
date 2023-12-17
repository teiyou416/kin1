/*CxgÄàÊmiINEThC[RlNV^j*/
/*eíè`*/
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

/*C[`iNCAgj*/
int main(int argc, char **argv)
{

  int sofd;                   /* \PbgLqq*/
  struct hostent *shost;      /* hostent\¢Ì*/
  struct sockaddr_in sv_addr; /* sockaddr_in\¢Ì */

  /*\PbgÌì¬iTCP)    
     ó 
@@@ó
@@@ó
@ */sofd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     


  /*T[oÌAhXðæŸ
@@@ó
@@@ó
@@@ó
  */  shost = gethostbyname("10.0.0.20");
      if (shost == NULL) {
      exit(EXIT_FAILURE);
}


  


  /*T[oÌAhXðÝè*/
  bzero((void *)&sv_addr,sizeof(sv_addr));
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(PORT_NO);
  memcpy((void *)&sv_addr.sin_addr,(void *)shost->h_addr,shost->h_length);
  
  /*\PbgÌÚ±v
     ó
@@@ó
@@@ó
  */if (connect(sofd, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
    perror("yes");
      exit(EXIT_FAILURE);
}
  

  cliepro(sofd);
  
  close(sofd);

  exit(0);
}

/*[`iNCAgj*/
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
