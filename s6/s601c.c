/*イベント案内通知（INETドメインーコネクション型）*/
/*各種定義*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>

#define PORT_NO 8001
#define MAX_MSG 400
#define MAX_HOSTNAME 20

#define TopMessage "Enter Message\n"

typedef enum _ftype {
  UsrMsg, AdminMsg
} ftype;

typedef struct _Frame {
  ftype type;
  char hostname[MAX_HOSTNAME];
  char msg[MAX_MSG];
} Frame;

typedef struct cpos{
  int y;
  int x;
} cpos;

void cliepro(int);
void printTop(void);
void ClearScreen(void);
int getMessageFromTop(char *, int);
void setFrameToBody(Frame *);
int sendFrame(int, char *, Frame *);
void setErrorToBody(char *);

cpos top, body;

/*メインルーチン（クライアント）*/
int main(int argc, char **argv)
{

  int sofd;                   /* ソケット記述子*/
  struct hostent *shost;      /* hostent構造体*/
  struct sockaddr_in sv_addr; /* sockaddr_in構造体 */

  /*ソケットの作成（TCP) */
    
  sofd=socket(AF_INET,SOCK_STREAM,0);
  if(sofd<0) {
    perror("socket");
    exit(-1);
  }

  /*サーバのアドレスを取得 */
  shost = gethostbyname(argv[1]);
  if (shost == NULL) {
    perror("gethostbyname");
    exit(-1);
  }

  /*サーバのアドレスを設定*/
  bzero((void *)&sv_addr,sizeof(sv_addr));
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(PORT_NO);
  memcpy((void *)&sv_addr.sin_addr,(void *)shost->h_addr,shost->h_length);
  
  /*ソケットの接続要求*/
  if (connect(sofd, (struct sockaddr *)&sv_addr, sizeof(sv_addr))<0) {
    perror("connect");
    exit(-1);
  }

  cliepro(sofd);
  
  close(sofd);
  exit(0);
}

/*処理ルーチン（クライアント）*/
void cliepro(int sofd)
{
  int cc,nbyte, MAXRMSG;
  char smsg[MAX_MSG];
  Frame sndFrame, rcvFrame;
  fd_set readfds;
  int n;
  char hostname[MAX_HOSTNAME];

  if (gethostname(hostname, sizeof(hostname)) < 0) {
    perror("gethostname");
    exit(-1);
  }
  
  ClearScreen();

  while(1) {

    printTop();

    /* readfdsを初期化 */
    FD_ZERO(&readfds);
    /* 標準入力をreadfdsにセットする*/
    FD_SET(0, &readfds);
    /* ソケットをreadfdsにセットする */
    FD_SET(sofd, &readfds);

    /* 入力要求 */

    /* 標準入力とソケットのデータを検査する */
    if ((n = select(FD_SETSIZE, &readfds, NULL, NULL, NULL)) == -1) {
      perror("select");
      exit(-1);
    }

    /* 標準入力からのデータがあれば，データを処理する */
    if (n >0 && FD_ISSET(0, &readfds)) {
      n--;
      memset(&sndFrame, 0, sizeof(sndFrame));
      if (getMessageFromTop(sndFrame.msg, sizeof(sndFrame.msg)) < 0) return;
      if (sendFrame(sofd, hostname, &sndFrame) < 0) {
	setErrorToBody("Connection Down");
	return;
      }
    }

    /* ソケットにデータ受信があれば，データを処理する */
    if (n > 0 && FD_ISSET(sofd, &readfds)) {
      n--;
      memset(&rcvFrame, 0, sizeof(rcvFrame));
      cc = recv(sofd, &rcvFrame, sizeof(rcvFrame), 0);
      switch(cc) {
      case -1:
	perror("recv");
	exit(-1);
      case 0:
	setErrorToBody("Connection Down");
	return;
      default:
	setFrameToBody(&rcvFrame);
      }
    }

  }
	
}

void printTop(void)
{

  printf("\033[%d;%dH",1, 1); // move to (0,0)
  printf("\033[0K"); // line clear
  printf("\033[30m"); // font black
  printf("Enter Message:"); 
  fflush(stdout);

}

void ClearScreen(void)
{

  char buf[10];

  body.y=2; body.x=1;

  printf("Client Start OK ? Press AnyKey\n");
  fgets(buf, 10, stdin);
  printf("\033[2J"); //clear
 
}


int getMessageFromTop(char *msg, int msg_len)
{

  if (fgets(msg, msg_len, stdin) == NULL) {
    printf("\033[49m"); // BG=usual
    printf("\033[2J"); //clear
    printf("\033[30m"); // font=black
    printf("\033[%d;%dH",1, 1); //move to (0,0)
    return(-1);
  }

  printf("\033[%d;%dH", body.y, 25); //move to (current, 25)
  printf("\033[30mown:\n");  //font=black
  printf("\033[%d;%dH", body.y+1, 25); // move to (current+1, 25)
  printf("%s", msg);
  body.y+=2;
  fflush(stdout);

  return(0);
}

void setFrameToBody(Frame *rcvFrame)
{

  switch(rcvFrame->type) {
  case UsrMsg:
    if (strlen(rcvFrame->hostname) == 0) return;
    printf("\033[32m");//font=Green
    break;
  case AdminMsg:
    printf("\033[31m");//font=Red
    break;
  default:
    setErrorToBody("Invalid Msg Type");
    return;
  }

  printf("\033[%d;%dH", body.y, body.x); //move to (current, 0)
  printf("%s:\n%s", rcvFrame->hostname, rcvFrame->msg);
  body.y+=2;
  fflush(stdout);

}

int sendFrame(int sofd, char *hostname, Frame *sndFrame)
{
  int rc;

  sndFrame->type = UsrMsg;
  strcpy(sndFrame->hostname, hostname);
  rc = send(sofd, sndFrame, sizeof(Frame), 0);

  return rc;

}

void setErrorToBody(char *err)
{

  printf("\033[%d;%dH", body.y, body.x); //move to (current, 0)
  printf("\033[40m\033[37m%s\n",err); //BG=black Font=White
  body.y++;
  printf("\033[49m");	// BG=usual
  printf("\033[30m");	//font=black
  fflush(stdin);

}


