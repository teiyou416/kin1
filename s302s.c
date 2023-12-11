/*イベント案内通知（INETドメインーコネクション型）*/
/*各種定義*/
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

/*メインルーチン（サーバ）*/
int main()
{
  int cadlen;
  int sofd, nsofd;
  struct hostent *shost, *chost;
  struct sockaddr_in sv_addr, cl_addr;
  char shostname[20];
  struct in_addr in;

  /*ソケットの作成（TCP）*/
  sofd=socket(AF_INET,SOCK_STREAM,0);
  if(sofd<0) {
    perror("socket");
    exit(-1);
  }
  
  /*ソケットに名前を付ける*/
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

  /*クライアントからの接続キューを作成*/
  if (listen(sofd,5)<0) {
    perror("listen");
    exit(-1);
  }

  while(1){
    /*クライアントからの接続を許可*/
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

/*処理ルーチン（サーバ）*/
void msgpro(int sofd, int nsofd, struct sockaddr_in cl)
{
  int cc,cadlen,nbyte, MAXRMSG;
  struct in_addr in;
  char rmsg[100];

  MAXRMSG=sizeof(rmsg);

  while(1){
    bzero(rmsg, MAXRMSG);
    if ((cc=recv(nsofd,rmsg,MAXRMSG,0))==0) break;
    printf("Received data = %s", rmsg);
    if(send(nsofd,rmsg,cc,0)<0)
      perror("send");
    
  }
  memcpy(&in.s_addr, &cl.sin_addr, 4);
  printf("Disconneted clinet %s:%d\n",inet_ntoa(in), ntohs(cl.sin_port));
  close(nsofd);

}


