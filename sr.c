/*CxgÄàÊmiINEThC[RlNV^j*/
/*eíè`*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define PORT_NO 8001

void msgpro(int, int, struct sockaddr_in);
void cliepro(int);

/*C[`iT[oj*/
int main()
{
  int cadlen;
  int sofd, nsofd;
  struct hostent *shost, *chost;
  struct sockaddr_in sv_addr, cl_addr;
  char shostname[20];
  struct in_addr in;

  /*\PbgÌì¬iTCPj*/
  sofd=socket(AF_INET,SOCK_STREAM,0);
  if(sofd<0) {
    perror("socket");
    exit(-1);
  }
  
  /*\PbgÉŒOðt¯é*/
  if(gethostname(shostname, sizeof(shostname))<0) {
    perror("gethostname");
    exit(-1);
  }
  
  shost = gethostbyname(shostname);
  if(shost==NULL) {
    perror("gethostbyname");
    exit(-1);
  }
  
  bzero((char *)&sv_addr,sizeof(sv_addr));
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(PORT_NO);
  memcpy((char *)&sv_addr.sin_addr,(char *)shost->h_addr,shost->h_length);

  if(bind(sofd,(struct sockaddr *)&sv_addr,sizeof(sv_addr))<0) {
    perror("bind");
    exit(-1);
  }

  /*NCAg©çÌÚ±L[ðì¬*/
  if (listen(sofd,5)<0) {
    perror("listen");
    exit(-1);
  }

  while(1){
    /*NCAg©çÌÚ±ðÂ*/
    cadlen = sizeof(cl_addr);
    if((nsofd=accept(sofd,(struct sockaddr *)&cl_addr, (socklen_t *)&cadlen))<0) {
      perror("accept");
      exit(-1);
    }
    memcpy(&in.s_addr, &cl_addr.sin_addr, 4);
    printf("Accept client %s:%d\n", inet_ntoa(in), ntohs(cl_addr.sin_port));
    msgpro(sofd, nsofd, cl_addr);
  }
}

/*[`iT[oj*/
void msgpro(int sofd, int nsofd, struct sockaddr_in cl)
{
  int cc,cadlen,nbyte, MAXRMSG;
  struct in_addr in;
  char rmsg[100];
  char rmsg1[100];
  MAXRMSG=sizeof(rmsg);
       
  while(1){
    bzero(rmsg, MAXRMSG);
    if ((cc=recv(nsofd,rmsg,MAXRMSG,0))==0) break;
    printf("Received data = %s", rmsg);
    if(send(nsofd,rmsg,cc,0)<0)
      perror("send");
    
  }
fgets(rmsg, sizeof(rmsg), stdin);
           int compare(const void *a,const void *b){
   return (*(char*)a-*(char*)b);
}
        nbyte = strlen(rmsg);
       qsort(rmsg, strlen(rmsg), sizeof(char), compare);
        
        if (send(sofd, rmsg, nbyte, 0) < 0) {
            perror("send");
        } else {
            cc = recv(sofd, rmsg, MAXRMSG, 0);
            if (cc < 0) 
                perror("recv");
            else {
                printf("%s", rmsg);
                bzero(rmsg, MAXRMSG);
            }
        }
  memcpy(&in.s_addr, &cl.sin_addr, 4);
  printf("Disconneted clinet %s:%d\n",inet_ntoa(in), ntohs(cl.sin_port));
  close(nsofd);

}
void cliepro(int sofd) {
    int cc, nbyte, MAXRMSG;
    char smsg[100], rmsg[100];

    MAXRMSG = sizeof(rmsg);
    bzero(rmsg, MAXRMSG);

    while (1) {
        printf("Enter string: ");
        fgets(smsg, sizeof(smsg), stdin);
        if (feof(stdin)) break;
           int compare(const void *a,const void *b){
   return (*(char*)a-*(char*)b);
}
        nbyte = strlen(smsg);
        qsort(smsg, strlen(smsg), sizeof(char), compare);
        
        if (send(sofd, smsg, nbyte, 0) < 0) {
            perror("send");
        } else {
            cc = recv(sofd, rmsg, MAXRMSG, 0);
            if (cc < 0) 
                perror("recv");
            else {
                printf("%s", rmsg);
                bzero(smsg, MAXRMSG);
            }
        }
    }
}
