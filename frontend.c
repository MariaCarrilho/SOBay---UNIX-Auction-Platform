#include "header_FE.h"
#include <ctype.h>
int terminate =0;
int try=0;

int login(int fd_backend_fifo, char *nome, char *pass, char nome_fifo_FE[], void *user1){

    struct user *myUser = (struct user * ) user1;
    struct user userCopy;

    strcpy(userCopy.username, nome);
    strcpy(userCopy.password, pass);
    userCopy.userPid = getpid();
    userCopy.tipo=0;

    strcpy(myUser->username, nome);
    strcpy(myUser->password, pass);
    myUser->userPid = getpid();

    int sizeEnv = write(fd_backend_fifo,&userCopy, sizeof(userCopy));
    if(sizeEnv<=0) {
        printf("Erro a enviar credenciais de login\n");
        return -1;
    }
    int fd_resposta = open(nome_fifo_FE,O_RDONLY);
    int resp;
    int s2 = read(fd_resposta,&resp,sizeof(resp));
    close(fd_resposta);
    if(resp == 1) printf("Utilizador não está registado, try again\n");
    else if(resp == 2) printf("Utilizador já autenticado, try again\n");
    else if(resp==0) printf("Utilizador registado, moving on\n");
    return resp;
}

void endThreads(int sign, siginfo_t *info, void *old){
    printf("Expulso pelo administrador\nGoodbye\n");
    terminate=1;
}

void *recebeNotif(void * l){
    struct heartbeat *h = (struct heartbeat * ) l;
    printf("Entrei na thread das notificações\n");
    char nome_fifo_NOTIF[100], novidade[500];
    sprintf(nome_fifo_NOTIF, BE_NOTIF_CLI, getpid());
    if(mkfifo(nome_fifo_NOTIF,0666)==-1){
        printf("Erro ao abrir fifo");
        exit(0);
    }
    int fd_fifo = open(nome_fifo_NOTIF,O_RDWR);
    if(fd_fifo==-1) {
        printf("Erro a abrir fifo\n");
        exit(0);
    }
    do{
        int size = read(fd_fifo,&novidade,sizeof(novidade));
        if(size>0){ printf("%s\n", novidade); }
    } while (h->ativo!=1);
    close(fd_fifo);
    unlink(nome_fifo_NOTIF);
}

void *heartbeat(void * l){
    struct heartbeat *h = (struct heartbeat * ) l;
    struct user structuser;
    structuser.userPid = getpid();
    structuser.tipo=2;
    do{
        printf("A enviar heartbeat\n");
        int sizeEnv = write(h->fd,&structuser, sizeof(structuser));
        if(sizeEnv<=0) {
            printf("Erro a enviar heartbeat\n");
            break;
        }
        sleep(h->nSec);
    }while(h->ativo!=1);
    close(h->fd);
}

void termina(int s,siginfo_t *t, void *p){}


int main(int argc, char **argv){
    setbuf(stdout, NULL);
    struct user myUser;
    pthread_t heart, notif;
    char *secondWord, *thirdWord, *aux, delim[]=" ", *pBase,*compreJa, *dur,*myfifo = "myfifo";
    int aux_isdigit=0;
    char nome_fifo_FE[100],username[100], password[100];
    union sigval val;

    char *sec = getenv("HEARTBEAT");
    if(sec==NULL) {
        printf("Erro na variável hearbeat\n");
        return -1;
    }
    //Falta verificação p garantir que é um inteiro
    int nSegundos = atoi(sec);
    
    if(argc!=3){
        printf("Wrong number of arguments\n");
        exit(0);
    }
    
    int fd_backend_fifo = open (BE_FIFO,O_WRONLY);
    if (fd_backend_fifo == -1){
        printf("Erro, o servidor não está a correr\n");
        return -1;
    }

    sprintf(nome_fifo_FE, FE_FIFO, getpid());
    if(mkfifo(nome_fifo_FE,0666)==-1){
        printf("Erro ao abrir fifo");
        return 1;
    }


    int login1 = login(fd_backend_fifo, argv[1], argv[2], nome_fifo_FE, &myUser);
    if(login1==-1) return -1;
    while(login1!=0) {
        if(try==2){
            printf("Demasiadas Tentativas\nGoodbye\n");
            exit(0);
        }
        try++;
        printf("Username:");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = '\0';
        printf("Password:");
        fgets(password, sizeof(password), stdin);
        password[strcspn(password,"\n")] = '\0';
        login1 = login(fd_backend_fifo, username, password, nome_fifo_FE,&myUser);
    }

    struct heartbeat h;
    h.ativo=0;
    h.pid=getpid();
    h.fd = fd_backend_fifo;
    h.nSec = nSegundos;
    if(pthread_create(&heart, NULL, &heartbeat, &h)!=0)
        return -1;
    if(pthread_create(&notif, NULL, &recebeNotif, &h)!=0)
        return -1;

    do{
        struct sigaction sa1;
        sa1.sa_sigaction = endThreads;
        sa1.sa_flags = SA_SIGINFO;
        sigaction(SIGINT, &sa1, NULL);

        struct sigaction sa;
        sa.sa_sigaction = termina;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR2, &sa, NULL);

        myUser.tipo=1;
        /*printf("%d\n", myUser.userPid);
        printf("%s\n", myUser.username);
        printf("%s\n", myUser.password);*/
        char command[100];
        printf("Command: ");
        fgets(command, sizeof(command), stdin);
        secondWord=NULL; thirdWord=NULL;

        aux = strtok(command, delim);

        strcpy(myUser.comando,aux); // insere o comando na estrutura para ser enviado !!! importante era o que estava a falhar no envio para o backend
        aux_isdigit=0;
        if(strstr(aux, "sell")){
            secondWord = strtok(NULL, delim);
            thirdWord = strtok(NULL, delim);
            pBase = strtok(NULL, delim);
            compreJa = strtok(NULL, delim);
            dur = strtok(NULL, delim);
            if(secondWord==NULL||compreJa==NULL || dur==NULL||pBase==NULL || thirdWord==NULL)
                printf("Comando Inválido ou parametros incorretos\n");
            else{
                sprintf(myUser.comando, "sell %s %s %s %s %s", secondWord, thirdWord, pBase, compreJa, dur);
for(int i=0;i<strlen(pBase)-1;i++){
                    if(isdigit(secondWord[i])==0){aux_isdigit=1;break;}
                }
for(int i=0;i<strlen(compreJa)-1;i++){
                    if(isdigit(secondWord[i])==0){aux_isdigit=1;break;}
                }
for(int i=0;i<strlen(dur)-1;i++){
                    if(isdigit(secondWord[i])==0){aux_isdigit=1;break;}
                }
                if(aux_isdigit==1){printf("Comando Inválido\n");}
else{
                myUser.comando[strcspn(myUser.comando, "\n")] = '\0';
                int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE,O_RDWR);

                int s2= read(fd_resposta,&myUser,sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }}
        }//está
        else if(strstr(aux, "cash")){
            int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
            int fd_resposta = open(nome_fifo_FE,O_RDWR);
             	
    		int s2= read(fd_resposta,&myUser,sizeof(myUser));
    		close(fd_resposta);
            printf("%s\n", myUser.resposta);
        }//está
        else if(strstr(aux, "list")){
            secondWord = strtok(NULL, delim);
            if(secondWord!=NULL) printf("Comando Inválido\n");
            else{
                int sizeEnv = write(fd_backend_fifo, &myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE, O_RDWR);

                int s2 = read(fd_resposta, &myUser, sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }
        }//está
        else if(strstr(aux, "licat")){
            secondWord = strtok(NULL, delim);
            if(secondWord==NULL) printf("Comando Inválido\n");
            else{
                sprintf(myUser.comando, "licat %s", secondWord);
                myUser.comando[strcspn(myUser.comando, "\n")] = '\0';
                int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE,O_RDWR);

                int s2= read(fd_resposta,&myUser,sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }
        }//está
        else if(strstr(aux, "lisel")){
            secondWord = strtok(NULL, delim);
            if(secondWord==NULL) printf("Comando Inválido\n");
            else{
                sprintf(myUser.comando, "lisel %s", secondWord);
                myUser.comando[strcspn(myUser.comando, "\n")] = '\0';
                int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE,O_RDWR);

                int s2= read(fd_resposta,&myUser,sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }
        }//está
        else if(strstr(aux, "litime")){
            secondWord = strtok(NULL, delim);
            if(secondWord==NULL) printf("Comando Inválido\n");
            else{
                sprintf(myUser.comando, "litime %s", secondWord);
for(int i=0;i<strlen(secondWord)-1;i++){
                    if(isdigit(secondWord[i])==0){aux_isdigit=1;break;}
                }
                if(aux_isdigit==1){printf("Comando Inválido\n");}
else{
                myUser.comando[strcspn(myUser.comando, "\n")] = '\0';
                int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE,O_RDWR);

                int s2= read(fd_resposta,&myUser,sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }}
        }//está
        else if(strstr(aux, "lival")){
            secondWord = strtok(NULL, delim);
            if(secondWord==NULL) printf("Comando Inválido\n");
            else{
                sprintf(myUser.comando, "lival %s", secondWord);
for(int i=0;i<strlen(secondWord)-1;i++){
                    if(isdigit(secondWord[i])==0){aux_isdigit=1;break;}
                }
                if(aux_isdigit==1){printf("Comando Inválido\n");}
else{
                myUser.comando[strcspn(myUser.comando, "\n")] = '\0';
                int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE,O_RDWR);

                int s2= read(fd_resposta,&myUser,sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }}
        }//está
        else if(strstr(aux, "add")){
            secondWord = strtok(NULL, delim);
            if(secondWord==NULL) printf("Comando Inválido\n");
            else{
                sprintf(myUser.comando, "add %s", secondWord);
                for(int i=0;i<strlen(secondWord)-1;i++){
                    if(isdigit(secondWord[i])==0){aux_isdigit=1;break;}
                }
                if(aux_isdigit==1){printf("Comando Inválido\n");}
else{
                int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE,O_RDWR);

                int s2= read(fd_resposta,&myUser,sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }
}
        }//está
        else if(strstr(aux, "time")){
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            printf("Time: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year +1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        }//está
        else if(strstr(aux, "buy")){
            secondWord = strtok(NULL, delim);
            thirdWord = strtok(NULL, delim);
printf("second word thirdword: [%s] [%s]",secondWord,thirdWord);
            if(secondWord==NULL || thirdWord ==NULL) {
                printf("Comando Inválido\n");
            }else{
                sprintf(myUser.comando, "buy %s %s", secondWord, thirdWord);
                for(int i=0;i<strlen(secondWord)-1;i++){
                    if(isdigit(secondWord[i])==0){aux_isdigit=1;break;}
                }
                for(int i=0;i<strlen(thirdWord)-1;i++){
                    if(isdigit(thirdWord[i])==0){aux_isdigit=1;break;}
                }
                if(aux_isdigit==1){printf("Comando Inválido\n");}
            else{
                int sizeEnv = write(fd_backend_fifo,&myUser, sizeof(myUser));
                int fd_resposta = open(nome_fifo_FE,O_RDWR);

                int s2= read(fd_resposta,&myUser,sizeof(myUser));
                close(fd_resposta);
                printf("%s\n", myUser.resposta);
            }
            }
        }
        else if(strstr(aux, "exit")){
            myUser.tipo=3;
            int sizeEnv = write(fd_backend_fifo, &myUser, sizeof(myUser));
            int fd_resposta = open(nome_fifo_FE, O_RDWR);
            close(fd_resposta);
            terminate=1;
        }
        else printf("Comando Inválido\n");

    }while(terminate!=1);

    unlink(FE_FIFO);
    unlink(BE_NOTIF_CLI);
    h.ativo=1;

    if (pthread_kill(heart, SIGUSR2) != 0)
        return -1;
    if (pthread_kill(notif, SIGUSR2) != 0)
        return -1;
    if(pthread_join(heart, NULL)!=0)
        return -1;
    if(pthread_join(notif, NULL)!=0)
        return -1;
    return 0;
}
