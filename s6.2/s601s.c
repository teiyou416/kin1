#include <arpa/inet.h>
#include <ctype.h>
#include <linux/limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h> /* select()でTimeOutを設定する場合必要． */
#include <sys/types.h>
#include <unistd.h>
#define PORT_NO 8001 /* サーバのポート番号 */
#define PORT_Super 9001
#define MAX_CLIENTS 10 /* 同時接続最大クライアント数 */
#define Err(x)                                                                 \
    {                                                                          \
        fprintf(stderr, "server.- ");                                          \
        perror(x);                                                             \
        exit(0);                                                               \
    }
#define MAX_MSG 400
#define MAX_HOSTNAME 20

/* クライアント構造体：接続したクライアントの情報を格納する構造体　*/
typedef struct client {
    struct client *next; /* 次の要素へアドレス */
    int so; /* 接続確立したクライアントのソケットディスクリプタ */
    struct sockaddr_in addr; /* クライアントのIPアドレス，ポート番号 */
    char *h_name;            /* クライアントのホスト名    */
    int msg_count;
    size_t msg_len;
    int superuser;
} Client;

/* クライアントリストメタ情報：クライアント構造体を要素とするリスト構造の制御情報
 */
typedef struct meta {
    Client *
        pclient_head; /* 情報を格納している要素リスト(クライアントリスト)の先頭アドレス
                       */
    Client *
        pclient_tail; /* 情報を格納している要素リスト(クライアントリスト）の終端アドレス
                       */
    int num_clients; /* 情報を格納している要素数（＝接続クライアント数）*/
} Meta_Info;

typedef enum _ftype { UsrMsg, AdminMsg } ftype;

typedef struct _Frame {
    ftype type;
    char hostname[MAX_HOSTNAME];
    char msg[MAX_MSG]; // MAX_MSG
} Frame;

Meta_Info *init_MetaInfo(void);
void sockfd_set(fd_set *, int, Meta_Info *);
void accept_to_connect(int, fd_set *, Meta_Info *);
void receive_data(fd_set *, Meta_Info *);
// void echo_back(int, Frame *, int); // 课题1
void echo_other(Client *, Meta_Info *, Frame *, int);
char *getclientname(struct sockaddr_in *);
void message_cli(Client *, Meta_Info *);
void in_Cli(Client *, Meta_Info *);
void note_Cli(Client *, Meta_Info *);
void superU(Client *, Meta_Info *);
Client *find_Cli(Meta_Info *, char *);
void kick_out(Client *, Meta_Info *);
int main(void) {

    Meta_Info *pmeta;
    int connsd; /* 接続要求受信用のソケットディスクリプタ*/
    fd_set readfds; /* 読み込み用のファイルディスクリプタの集合体 */
    struct sockaddr_in saddr; /*　サーバのbind用の構造体 */
    struct sockaddr_in caddr; /* 接続要求したクライアントのアドレス情報 */
    int n;

    /* クライアントリストメタ情報の初期化 */
    pmeta = init_MetaInfo();

    /*　接続要求受信用のソケットの生成　*/
    if ((connsd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    /*saddrの初期化*/
    bzero((char *)&saddr, sizeof(saddr));

    /*　利用するネットワークの設定  */
    saddr.sin_family = AF_INET;                /* IP */
    saddr.sin_addr.s_addr = htonl(INADDR_ANY); /* すべてのIPネットワーク*/
    saddr.sin_port = htons(PORT_NO);           /* ポート番号*/

    if (bind(connsd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("bind");
        exit(1);
    }

    /* 接続要求を受けるバッファを用意する listen() */
    /* 第2引数は大きめの値にする */
    if (listen(connsd, 5) == -1) {
        perror("listen");
        exit(1);
    }

    /* メインループ */
    while (1) {

        /*
    　　　接続要求受信用ソケットディスクリプタと接続クライアントからの
    　　　データ受信用ソケットディスクリプタをreadfdsにセットする
        */
        sockfd_set(&readfds, connsd, pmeta);

        /*
         * 接続要求受信とデータ受信を確認指示をする
         */
        if ((n = select(FD_SETSIZE, &readfds, NULL, NULL, NULL)) == -1) {
            perror("select");
            exit(1);
        }

        /* 先ず，優先して接続要求があれば，接続処理を行う */
        accept_to_connect(connsd, &readfds, pmeta);

        /* 次に，データ受信があれば，データ受信処理を行う */
        receive_data(&readfds, pmeta);
    }
}

Meta_Info *init_MetaInfo(void) {
    Client *p;
    Meta_Info *m;
    int i;

    m = (Meta_Info *)malloc(sizeof(Meta_Info));
    if (m == NULL) {
        perror("malloc");
        exit(-1);
    }

    /* Meta_Infoの初期化　*/
    m->pclient_head = NULL;
    m->pclient_tail = NULL;
    m->num_clients = 0;

    return (m); /* 確保したクライアント構造体メタ情報のアドレスを返す */
}

void sockfd_set(fd_set *rfds, int connsd, Meta_Info *pmeta) {
    Client *p, *pprev;
    int i;
    /* readfdsの初期化*/
    FD_ZERO(rfds);

    /* 最初に，接続要求受信用ソケットディスクリプタをセットする */
    FD_SET(connsd, rfds);

    /* 接続クライアントとのソケットディスクリプタをセットする */
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        FD_SET(p->so, rfds);
    }
}

void accept_to_connect(int connsd, fd_set *rfds, Meta_Info *pmeta) {
    int len, i;
    int so;
    struct sockaddr_in caddr;
    struct in_addr in;
    Client *p;

    if (FD_ISSET(connsd, rfds)) { /* 接続要求の受信確認 */

        len = sizeof(caddr);
        so = accept(connsd, (struct sockaddr *)&caddr, (socklen_t *)&len);

        if (pmeta->num_clients <
            MAX_CLIENTS) { /* 接続クライアント数のチェック */

            /* accept()でデータ送受信用のソケットディスクリプタが返されるのでこれをクライアント構造体に保存する
             */
            p = (Client *)malloc(
                sizeof(Client)); /* クライアント構造体の領域を確保 */
            if (p == NULL) {
                perror("malloc");
                exit(-1);
            }
            p->so = so; /* データ送受信用ソケットディスクリプタ*/
            memcpy(&(p->addr), &caddr,
                   sizeof(caddr)); /* クライアントのIPアドレス，ポート番号*/
            p->h_name = getclientname(&caddr); /* クライアントのホスト名 */

            /* クライアントリストの終端に新規接続クライアント構造体を繋ぐ */
            if (ntohs(caddr.sin_port) == PORT_Super) {
                p->superuser = 1;

            } else {
                p->superuser = 0;
            }

            if (pmeta->pclient_tail != NULL) {
                p->next = pmeta->pclient_tail->next;
                pmeta->pclient_tail->next = p;
            } else {
                p->next = pmeta->pclient_tail;
            }
            pmeta->pclient_tail = p;
            /* pclient_head == NULL
             * の場合（クライアントリストが空)，pclient_headに繋ぐ */
            if (pmeta->pclient_head == NULL) {
                p->next = pmeta->pclient_head;
                pmeta->pclient_head = p;
            }
            pmeta->num_clients++; /* 接続クライアント数の更新 */
            memcpy(&in.s_addr, &(caddr.sin_addr), 4);
            fprintf(stdout, "Join client: %s IP=%s Port=%d)\n", p->h_name,
                    inet_ntoa(in), ntohs(caddr.sin_port));
            in_Cli(p, pmeta);
            note_Cli(p, pmeta);
        } else {
            memcpy(&in.s_addr, &(caddr.sin_addr), 4);
            fprintf(stderr, "Reject client: IP=%s Port=%d\n", inet_ntoa(in),
                    ntohs(caddr.sin_port));
            close(so);
        }
    }
}

void receive_data(fd_set *rfds, Meta_Info *pmeta) {
    Client *p, *q, *prev;
    int msglen;
    struct in_addr in;
    Frame buf;

    /* クライアントリストに繫がっている各クライアント構造体のソケットディスクリプタからのデータ受信を確認する．*/
    for (p = pmeta->pclient_head, prev = NULL; p != NULL; p = p->next) {

        if (FD_ISSET(p->so, rfds)) {
            bzero(&buf, sizeof(buf));
            msglen = recv(p->so, &buf, sizeof(Frame), 0);
            switch (msglen) {
            case -1:
                perror("read");
                exit(-1);
            case 0: /* クライアントが接続を切断 */
                memcpy(&in.s_addr, &(p->addr.sin_addr), 4);
                fprintf(stdout, "Leave client: %s IP=%s Port=%d\n", p->h_name,
                        inet_ntoa(in), ntohs(p->addr.sin_port));
                int msg_count = p->msg_count;
                int byte_count = p->msg_len;
                fprintf(stdout, "%s sent message:%d word:%d \n", p->h_name,
                        p->msg_count, byte_count);
                message_cli(p, pmeta);
                close(p->so);
                /* クライアントリストから削除 */
                if (prev == NULL) { /* リストの先頭の場合　*/
                    pmeta->pclient_head = p->next;
                } else {
                    prev->next = p->next;
                }
                if (p == pmeta->pclient_tail) { /* リストの終端の場合 */
                    pmeta->pclient_tail = prev;
                }

                pmeta->num_clients--;
                free(p->h_name);
                free(p);
                p->msg_count = 0;
                p->msg_len = 0;
                p = prev; /* pが示す領域はリストから削除されて領域が解放されたので
                             pをクライアントリストの前方クライアントの領域を示すようにする*/
                break;

            default: /* データ受信成功 */
                memcpy(&in.s_addr, &(p->addr.sin_addr), 4);
                if (p->superuser == 1) {
                    fprintf(stdout, "**Administrator**");
                    if (strcmp(buf.msg, "status") == 0) {
                        superU(p, pmeta);
                    }
                    if (strncmp(buf.msg, "quit+", 5) == 0) {
                        char *username = buf.msg + 5;
                        Client *kicked_client = find_Cli(pmeta, username);
                        if (kicked_client != NULL) {
                            kick_out(kicked_client, pmeta);
                        }
                    }
                }

                fprintf(stdout, "client %s IP=%s Port=%d:\"%s:%s\"\n",
                        p->h_name, inet_ntoa(in), ntohs(p->addr.sin_port),
                        buf.hostname, buf.msg);
                p->msg_len += strlen(buf.msg);
                // echo_back(p->so, &buf, msglen); // 课题1
                echo_other(p, pmeta, &buf, msglen);
                p->msg_count++;
                break;
            }
        }
        if (p == NULL)
            break; /* 領域解放してリストから空になる場合 */
        prev =
            p; /* 次のループの処理のためにリスト前方のクライアントとして保持 */
    }
}
void kick_out(Client *kicked_client, Meta_Info *pmeta) {
    Client *p, *prev;
    char kicked_username[MAX_HOSTNAME];
    strcpy(kicked_username, kicked_client->h_name);
    close(kicked_client->so);
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (p == kicked_client) {
            if (prev == NULL) { /* リストの先頭の場合　*/
                pmeta->pclient_head = p->next;
            } else {
                prev->next = p->next;
            }
            if (p == pmeta->pclient_tail) { /* リストの終端の場合 */
                pmeta->pclient_tail = prev;
            }

            pmeta->num_clients--;
            free(p->h_name);
            free(p);
            break;
        }
        prev = p;
    }
    char msg[MAX_MSG];
    snprintf(msg, MAX_MSG, "***%s HAVE BEEN KICED OUT BY Administrator***",
             kicked_username);
    Frame kickFrame;
    memset(&kickFrame, 0, sizeof(Frame));
    kickFrame.type = AdminMsg;
    strcpy(kickFrame.hostname, "Server");
    strcpy(kickFrame.msg, msg);
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (p != kicked_client) {
            write(p->so, &kickFrame, sizeof(Frame));
        }
    }
}
// void echo_back(int so, Frame *buf, int cbuf) { write(so, buf, cbuf); }
void echo_other(Client *sender, Meta_Info *pmeta, Frame *buf, int cbuf) {
    Client *p;
    int msg_size = sizeof(Frame) - sizeof(buf->hostname) + strlen(buf->msg) + 1;
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (p != sender) {
            write(p->so, buf, cbuf);
        }
    }
} // 课题1
void message_cli(Client *exiting_client, Meta_Info *pmeta) {
    Client *p;
    Frame exitFrame;
    memset(&exitFrame, 0, sizeof(Frame));
    exitFrame.type = AdminMsg;
    strcpy(exitFrame.hostname, exiting_client->h_name);
    sprintf(exitFrame.msg, "has left the chat.(IP=%s Port=%d)",
            inet_ntoa(exiting_client->addr.sin_addr),
            ntohs(exiting_client->addr.sin_port));
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (p != exiting_client) {
            write(p->so, &exitFrame, sizeof(Frame));
        }
    }
} // 2
void in_Cli(Client *now_client, Meta_Info *pmeta) {
    Client *p;
    Frame nowFrame;
    memset(&nowFrame, 0, sizeof(Frame));
    nowFrame.type = AdminMsg;
    strcpy(nowFrame.hostname, now_client->h_name);
    sprintf(nowFrame.msg, "Welcome to the chat.(IP=%s Port=%d)",
            inet_ntoa(now_client->addr.sin_addr),
            ntohs(now_client->addr.sin_port));
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (p != now_client) {
            write(p->so, &nowFrame, sizeof(Frame));
        }
    }
}
void note_Cli(Client *new_client, Meta_Info *pmeta) {
    Client *p;
    Frame newFrame;
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (p != new_client) {

            memset(&newFrame, 0, sizeof(Frame));
            newFrame.type = AdminMsg;
            strcpy(newFrame.hostname, p->h_name);
            sprintf(newFrame.msg, "is now in chat.(IP=%s Port=%d)",
                    inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port));

            write(new_client->so, &newFrame, sizeof(Frame));
        }
    }
}
Client *find_Cli(Meta_Info *pmeta, char *username) {
    Client *p;
    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (strcmp(p->h_name, username) == 0) {
            return p;
        }
    }
    return NULL;
}

void superU(Client *su_client, Meta_Info *pmeta) {
    Client *p;
    Frame suFrame;

    for (p = pmeta->pclient_head; p != NULL; p = p->next) {
        if (p != su_client) {
            memset(&suFrame, 0, sizeof(Frame));
            suFrame.type = AdminMsg;
            strcpy(suFrame.hostname, p->h_name);
            sprintf(suFrame.msg, "client  IP=%s Port=%d",
                    inet_ntoa(p->addr.sin_addr), ntohs(p->addr.sin_port));
            write(su_client->so, &suFrame, sizeof(Frame));
        }
    }
}

char Unknown[] = "Unknown";

char *getclientname(struct sockaddr_in *addr) {
    struct hostent *host;
    char *rc;

    host = gethostbyaddr(&addr->sin_addr, 4, AF_INET);

    if (host == NULL)
        return Unknown;

    if ((rc = (char *)malloc(strlen(host->h_name) + 1)) == NULL) {
        perror("malloc");
        exit(-1);
    }
    strcpy(rc, host->h_name);

    return (rc);
}
