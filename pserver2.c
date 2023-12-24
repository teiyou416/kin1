
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include<pthread.h>
#define PORT_NO 8001

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
void msgpro(int sofd, int nsofd, struct sockaddr_in cl)
{
int compare(const void *a,const void *b){
    return (*(char*)a-*(char*)b);
  }

  int cc,cadlen,nbyte, MAXRMSG;
  struct in_addr in;
  char rmsg[100];
  MAXRMSG=sizeof(rmsg);
while(1){
    bzero(rmsg, MAXRMSG);
    if ((cc=recv(nsofd,rmsg,MAXRMSG,0))==0) break;
    printf("Received data = %s", rmsg);

    qsort(rmsg,strlen(rmsg),sizeof(char),compare);
       nbyte=strlen(rmsg);
    if (send(nsofd,rmsg,nbyte,0)<0){
	perror("send");
   }else {
      cc=recv(sofd,rmsg,MAXRMSG,0);
      if(cc<0) perror("recv");
      else {
        rmsg[cc]='\0';
	printf("%s\n",rmsg);
	bzero(rmsg,MAXRMSG);
      }
    }

 }
memcpy(&in.s_addr, &cl.sin_addr, 4);
  printf("Disconneted clinet %s:%d\n",inet_ntoa(in), ntohs(cl.sin_port));
  close(nsofd);

}



int main()
{
  int cadlen;
  int sofd, nsofd;
  struct hostent *shost, *chost;
  struct sockaddr_in sv_addr, cl_addr;
  char shostname[20];
  struct in_addr in;

  if (signal(SIGCHLD, catch_sigchild) != 0) {
    perror("Signal");
    exit(-1);
  }


  sofd=socket(AF_INET,SOCK_STREAM,0);
  if(sofd<0) {
    perror("socket");
    exit(-1);
  }
  

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


  if (listen(sofd,5)<0) {
    perror("listen");
    exit(-1);
  }

  while(1){

    cadlen = sizeof(cl_addr);
    if((nsofd=accept(sofd,(struct sockaddr *)&cl_addr, (socklen_t *)&cadlen))<0) {
      perror("accept");
      exit(-1);
    }
pid_t pid =fork();
if(pid<0){
perror("fork");
close(nsofd);
}else if(pid == 0){
close(sofd);
msgpro(sofd,nsofd,cl_addr);
exit(0);
}else{
close(nsofd);
}
    memcpy(&in.s_addr, &cl_addr.sin_addr, 4);
    printf("Accept client %s:%d\n", inet_ntoa(in), ntohs(cl_addr.sin_port));
    
      }
}




