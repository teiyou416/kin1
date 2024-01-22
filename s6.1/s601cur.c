/*CxgÄàÊmiINEThC[RlNV^j*/
/*eíè`*/
#include <arpa/inet.h>
#include <fcntl.h>
#include <ncursesw/curses.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT_NO 8001
#define MAX_MSG 400
#define MAX_HOSTNAME 20

typedef struct _Cwindow {
    WINDOW *inWin;
    WINDOW *outWin;
    int y, x, line;
    int M_color, U_color, A_color;
} Cwindow;

typedef enum _ftype { UsrMsg, AdminMsg } ftype;

typedef struct _Frame {
    ftype type;
    char hostname[MAX_HOSTNAME];
    char msg[MAX_MSG];
} Frame;

void cliepro(int);
Cwindow *initScreen(void);
void setStringToTopWin(Cwindow *, char *);
void getStringFromTopWin(Cwindow *, char *, int);
void setStringToBodyWin(Cwindow *, char *);
void setFrameToBodyWin(Cwindow *, Frame *);
int sendFrame(int, char *, Frame *, int);
void setErrorToBodyWin(Cwindow *, char *);

/*C[`iNCAgj*/
int main(int argc, char **argv) {

    int sofd;                   /* \PbgLqq*/
    struct hostent *shost;      /* hostent\¢Ì*/
    struct sockaddr_in sv_addr; /* sockaddr_in\¢Ì */

    /*\PbgÌì¬iTCP) */

    sofd = socket(AF_INET, SOCK_STREAM, 0);
    if (sofd < 0) {
        perror("socket");
        exit(-1);
    }

    /*T[oÌAhXðæŸ */
    shost = gethostbyname(argv[1]);
    if (shost == NULL) {
        perror("gethostbyname");
        exit(-1);
    }

    /*T[oÌAhXðÝè*/
    bzero((void *)&sv_addr, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(PORT_NO);
    memcpy((void *)&sv_addr.sin_addr, (void *)shost->h_addr, shost->h_length);

    /*\PbgÌÚ±v*/
    if (connect(sofd, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) < 0) {
        perror("connect");
        exit(-1);
    }

    cliepro(sofd);

    close(sofd);
    exit(0);
}

/*[`iNCAgj*/
void cliepro(int sofd) {
    int cc, nbyte, MAXRMSG;
    char smsg[MAX_MSG];
    Frame sndFrame, rcvFrame;
    fd_set readfds;
    int n;
    Cwindow *Cwin;
    char hostname[MAX_HOSTNAME];

    if (gethostname(hostname, sizeof(hostname)) < 0) {
        perror("gethostname");
        exit(-1);
    }

    Cwin = initScreen();
    while (1) {
        /* readfdsðú» */
        FD_ZERO(&readfds);
        /* WüÍðreadfdsÉZbg·é*/
        FD_SET(0, &readfds);
        /* \PbgðreadfdsÉZbg·é */
        FD_SET(sofd, &readfds);

        /* üÍv */
        setStringToTopWin(Cwin, "Enter Message: ");

        /* WüÍÆ\PbgÌf[^ðž·é */
        if ((n = select(FD_SETSIZE, &readfds, NULL, NULL, NULL)) == -1) {
            endwin();
            perror("select");
            exit(-1);
        }

        /* WüÍ©çÌf[^ª êÎCf[^ð·é
         */
        if (FD_ISSET(0, &readfds)) {
            memset(&sndFrame, 0, sizeof(sndFrame));
            getStringFromTopWin(Cwin, sndFrame.msg, sizeof(sndFrame.msg));
            setStringToBodyWin(Cwin, sndFrame.msg);
            int msglen = strlen(sndFrame.msg);
            if (sendFrame(sofd, hostname, &sndFrame, msglen) < 0) {
                setErrorToBodyWin(Cwin, "Connection Down");
                endwin();
                return;
            }
            setStringToBodyWin(Cwin, sndFrame.msg);
        }

        /* \PbgÉf[^óMª êÎCf[^ð·é
         */
        if (FD_ISSET(sofd, &readfds)) {
            memset(&rcvFrame, 0, sizeof(rcvFrame));
            cc = recv(sofd, &rcvFrame, sizeof(rcvFrame), 0);
            switch (cc) {
            case -1:
                endwin();
                perror("recv");
                exit(-1);
            case 0:
                setErrorToBodyWin(Cwin, "Connection Down");
                endwin();
                return;
            default:
                setFrameToBodyWin(Cwin, &rcvFrame);
            }
        }
    }
}

Cwindow *initScreen(void) {
    Cwindow *Cwin;
    WINDOW *H1, *H2, *H3;

    if (initscr() == NULL) {
        fprintf(stderr, "initscr() error\n");
        exit(-1);
    }

    Cwin = (Cwindow *)malloc(sizeof(Cwindow));
    if (Cwin == NULL) {
        perror("malloc");
        exit(-1);
    }

    start_color();

    getmaxyx(stdscr, Cwin->y, Cwin->x);

    H3 = newwin(1, Cwin->x, 0, 0);
    H1 = newwin(1, Cwin->x, 3, 0);
    H2 = newwin(1, Cwin->x, 6, 0);
    Cwin->inWin = newwin(2, Cwin->x, 1, 0);
    Cwin->outWin = newwin(Cwin->y - 7, Cwin->x, 7, 0); // 4
    scrollok(Cwin->inWin, TRUE);
    scrollok(Cwin->outWin, TRUE);

    wattrset(H1, A_BOLD);
    wattrset(H2, A_BOLD);
    wattrset(H3, A_BOLD);
    mvwprintw(H1, 0, 0, "----[Send Message]-----------");
    mvwprintw(H2, 0, 0, "----[Chat Messages]----------");
    mvwprintw(H3, 0, 0, "----[USER NAME:]-----------");
    wrefresh(H1);
    wrefresh(H2);
    wrefresh(H3);
    Cwin->M_color = 1;
    init_pair(Cwin->M_color, COLOR_WHITE, COLOR_BLACK);
    Cwin->U_color = 2;
    init_pair(Cwin->U_color, COLOR_GREEN, COLOR_BLACK);
    Cwin->A_color = 3;
    init_pair(Cwin->A_color, COLOR_RED, COLOR_BLACK);

    Cwin->line = 0;

    return Cwin;
}

void setStringToTopWin(Cwindow *Cwin, char *msg) {

    werase(Cwin->inWin);
    mvwprintw(Cwin->inWin, 0, 0, "%s", msg);
    wrefresh(Cwin->inWin);
}

void getStringFromTopWin(Cwindow *Cwin, char *msg, int msg_len) {

    wgetnstr(Cwin->inWin, msg, msg_len);
    wrefresh(Cwin->inWin);
}

void setStringToBodyWin(Cwindow *Cwin, char *msg) {

    wattrset(Cwin->outWin, COLOR_PAIR(Cwin->M_color));

    if (Cwin->line < Cwin->y - 4) {
        mvwprintw(Cwin->outWin, Cwin->line, 0, "own:%s", msg);
        Cwin->line++;
    } else {
        wscrl(Cwin->outWin, 1);
        mvwprintw(Cwin->outWin, Cwin->y - 5, 0, "own:%s", msg);
    }
    wrefresh(Cwin->outWin);
}

void setFrameToBodyWin(Cwindow *Cwin, Frame *rcvFrame) {

    switch (rcvFrame->type) {
    case UsrMsg:
        if (strlen(rcvFrame->hostname) == 0)
            return;
        wattrset(Cwin->outWin, COLOR_PAIR(Cwin->U_color));
        break;
    case AdminMsg:
        wattrset(Cwin->outWin, COLOR_PAIR(Cwin->A_color));
        break;
    default:
        wattrset(Cwin->outWin, A_NORMAL);
    }

    if (Cwin->line < Cwin->y - 4) {
        mvwprintw(Cwin->outWin, Cwin->line, 0, "%s:%s", rcvFrame->hostname,
                  rcvFrame->msg);
        Cwin->line++;
    } else {
        wscrl(Cwin->outWin, 1);
        mvwprintw(Cwin->outWin, Cwin->y - 5, 0, "%s:%s", rcvFrame->hostname,
                  rcvFrame->msg);
    }
    wrefresh(Cwin->outWin);
}

int sendFrame(int sofd, char *hostname, Frame *sndFrame, int msglen) {
    int rc;

    sndFrame->type = UsrMsg;
    strcpy(sndFrame->hostname, hostname);
    rc =
        send(sofd, sndFrame, sizeof(Frame) - sizeof(sndFrame->msg) + msglen, 0);

    return rc;
}

void setErrorToBodyWin(Cwindow *Cwin, char *err) {

    wattrset(Cwin->outWin, A_STANDOUT);
    if (Cwin->line < Cwin->y - 4) {
        mvwprintw(Cwin->outWin, Cwin->line, 0, "Error:%s", err);
        Cwin->line++;
    } else {
        wscrl(Cwin->outWin, 1);
        mvwprintw(Cwin->outWin, Cwin->y - 5, 0, "Error:%s", err);
    }
    wrefresh(Cwin->outWin);

    sleep(1);
}
