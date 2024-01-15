#ifndef HEADER_FE_H
#define HEADER_FE_H

#define FE_FIFO "FRONTEND_FIFO_%d"
#define BE_FIFO "BACKENDFIFO"
#define BE_NOTIF_CLI "NOTIF_CLI_%d"

#define MAXUSERS 20
#define MAXPROM 10
#define MAXITEMS 30
#define MAXLEN 100

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>


struct user{
    char username[100],password[100];
    pid_t userPid;
    int ativo;
    char comando[100];
    char resposta[500];
    int tipo;
};

struct heartbeat{
    pid_t pid;
    int fd;
    int nSec;
    int ativo;
};

#endif
