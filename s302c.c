/*イベント案内通知（INETドメインーコネクション型）*/
/*各種定義*/
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

/*メインルーチン（クライアント）*/
int main(int argc, char **argv)
{

  int sofd;                   /* ソケット記述子*/
  struct hostent *shost;      /* hostent構造体*/
  struct sockaddr_in sv_addr; /* sockaddr_in構造体 */

  /*ソケットの作成（TCP)    
     空欄 
　　　空欄
　　　空欄
　 */sofd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
     


  /*サーバのアドレスを取得
　　　空欄
　　　空欄
　　　空欄
  */  shost = gethostbyname("10.0.0.20");
      if (shost == NULL) {
      exit(EXIT_FAILURE);
}


  


  /*サーバのアドレスを設定*/
  bzero((void *)&sv_addr,sizeof(sv_addr));
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(PORT_NO);
  memcpy((void *)&sv_addr.sin_addr,(void *)shost->h_addr,shost->h_length);
  
  /*ソケットの接続要求
     空欄
　　　空欄
　　　空欄
  */if (connect(sofd, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
    perror("yes");
      exit(EXIT_FAILURE);
}
  

  cliepro(sofd);
  
  close(sofd);

  exit(0);
}

/*処理ルーチン（クライアント）*/
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
	printf("%s",rmsg);
	bzero(rmsg,MAXRMSG);
      }
    }

  }
	
}
