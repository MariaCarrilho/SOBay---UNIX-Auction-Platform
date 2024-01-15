#ifndef HEADER_H
#define HEADER_H

#include "users_lib.h"
#include <time.h>
#include <stdio.h>
#include <math.h>
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
#include <ctype.h>

#define MAXUSERS 20
#define MAXPROM 10
#define MAXITEMS 30
#define MAXLEN 100

#define FE_FIFO "FRONTEND_FIFO_%d"
#define BE_FIFO "BACKENDFIFO"
#define BE_NOTIF_CLI "NOTIF_CLI_%d"

//frontend e backend
struct user{
    char username[100],password[100];
    pid_t userPid;
    int ativo;
    char comando[100];
    char resposta[500];
    int tipo;
};

//backend
struct login{
    int nUsers;
    struct user usersLogin[MAXUSERS];
};

//backend
struct prom{
    char promName[100];
    pid_t pid;
    int nPromos;
    char promo[100][100];
    int ativo;
    int checkItems;
};
//backend
struct item{
    int id;
    char nome[100];
    char categoria[100];
    int valorBase;
    int comprarJa;
    char userVende[100];
    int durLeilao;
    char userCompra[100];
    int emPromocao;
};

//backend
struct heartbeat_back{
    pid_t pid;
    time_t time1;
};

//backend
struct args{
    struct item lista[MAXITEMS];
    struct login login;
    struct heartbeat_back heartbeat_back[MAXUSERS];
    int ativo;
    int nSecHeart;
    int nItems;
    struct prom p[MAXPROM];
    int nPromotores;
    pthread_mutex_t  *m;
};



#endif
