#include "header.h"

int verifica_id(struct item lista[], int id, int n){
    for(int i =0; i< n;i++){
        if(id == lista[i].id)
            return 1;}
    return 0;
}

void erroVariavel(char varName[]){
    printf("Erro na variavel %s\n", varName);
    exit(0);
}

void fileNotExists(char fileName[]){
    printf("Ficheiro %s não existe\n", fileName);
    exit(0);
}

void mostra_info(struct item lista_items[], int n){
    for(int i=0; i< n;i++){
        printf("%d %s %s %d %d %d %s %s %d\n", lista_items[i].id, lista_items[i].nome, lista_items[i].categoria, lista_items[i].valorBase, lista_items[i].comprarJa, lista_items[i].durLeilao, lista_items[i].userVende, lista_items[i].userCompra, lista_items[i].emPromocao);
    }
}

char* lista_cliente(struct item lista_items[], int id, char *type, int n){
    char *lista;
    lista = (char *)malloc(500*sizeof(char)); //!!!importante lembrar de fazer free(lista)
    char aux[500];
    if(n==0) return "Atualmente não há items, come back later";
    if(id==0) {
        for(int i =0; i< n;i++){
            sprintf(aux, "%d %s %s %d %d %d %s %s %d\n", lista_items[i].id, lista_items[i].nome, lista_items[i].categoria,
                    lista_items[i].valorBase, lista_items[i].comprarJa, lista_items[i].durLeilao, lista_items[i].userVende,
                    lista_items[i].userCompra, lista_items[i].emPromocao);
            strcat(lista, aux);
        }
    } //todos
    else if(id==1){
        for(int i =0; i< n;i++){
            if(strcmp(lista_items[i].categoria, type)==0){
                sprintf(aux, "%d %s %s %d %d %d %s %s %d\n", lista_items[i].id, lista_items[i].nome, lista_items[i].categoria,
                        lista_items[i].valorBase, lista_items[i].comprarJa, lista_items[i].durLeilao, lista_items[i].userVende,
                        lista_items[i].userCompra, lista_items[i].emPromocao);
                strcat(lista, aux);
            }
        }
    } // categoria
    else if(id==2){
        for(int i =0; i< n;i++){
            if(strcmp(lista_items[i].userVende, type)==0){
                sprintf(aux, "%d %s %s %d %d %d %s %s %d\n", lista_items[i].id, lista_items[i].nome, lista_items[i].categoria,
                        lista_items[i].valorBase, lista_items[i].comprarJa, lista_items[i].durLeilao, lista_items[i].userVende,
                        lista_items[i].userCompra, lista_items[i].emPromocao);
                strcat(lista, aux);
            }

        }
    } // vendedor
    else if(id==3){
        int max = atoi(type);
        for(int i =0; i< n;i++){
            if(lista_items[i].valorBase <= max){
                sprintf(aux, "%d %s %s %d %d %d %s %s %d\n", lista_items[i].id, lista_items[i].nome, lista_items[i].categoria,
                        lista_items[i].valorBase, lista_items[i].comprarJa, lista_items[i].durLeilao, lista_items[i].userVende,
                        lista_items[i].userCompra, lista_items[i].emPromocao);
                strcat(lista, aux);
            }

        }
    } // preco maximo
    else if(id==4){
        int max = atoi(type);
        for(int i =0; i< n;i++){
            if(lista_items[i].durLeilao < max){
                sprintf(aux, "%d %s %s %d %d %d %s %s %d\n", lista_items[i].id, lista_items[i].nome, lista_items[i].categoria,
                        lista_items[i].valorBase, lista_items[i].comprarJa, lista_items[i].durLeilao, lista_items[i].userVende,
                        lista_items[i].userCompra, lista_items[i].emPromocao);
                strcat(lista, aux);
            }

        }
    } // duracao do leilao
    return lista;
}

void sendNot(struct args *arg, char msg[]){
    for(int i=0; i<arg->login.nUsers; i++){
        char nome_fifo_NOT[100];
        sprintf(nome_fifo_NOT, BE_NOTIF_CLI, arg->login.usersLogin[i].userPid);
        int fdEnvio = open(nome_fifo_NOT, O_WRONLY);
        int s2 = write(fdEnvio, msg, 200);
        close(fdEnvio);
    }

}

void removePromocoes(struct args *arg, char promName[]){
    char delim[]=" ",*aux, msg[500];
    for(int i=0; i<arg->nPromotores;i++){
        if(strcmp(promName,arg->p[i].promName)==0){
            for(int j=0; j<arg->p[i].nPromos;j++){
                aux = strtok(arg->p[i].promo[j], delim);
                int cash = atoi(strtok(NULL, delim));
                for(int k=0; k<arg->nItems; k++){
                    if(strcmp(aux, arg->lista[k].categoria)==0 && arg->lista[k].emPromocao==1){
                        arg->lista[k].emPromocao=0;
                        arg->lista[k].valorBase = arg->lista[k].valorBase+arg->lista[k].valorBase*(cash/100);
                        sprintf(msg, "Terminou a promoção com as especificações: %s", arg->p[i].promo[j]);
                        sendNot(arg, msg);
                    }
                }
            }
            arg->p[i].nPromos=0;
        }
    }

}

void removePromotor(struct args *arg, char promName[]){
    char name[100];
    for(int i=0; i<arg->nPromotores;i++){
        if(strcmp(promName,arg->p[i].promName)==0){
            printf("O promotor %s desapareceu\n", arg->p[i].promName);
            int pos=i;
            i= pos+1;
            while(i<=arg->nPromotores-1)
            {
                arg->p[i-1] = arg->p[i];
                i++;
            }
        }
    }
    removePromocoes(arg, promName);
    arg->nPromotores--;
}

void removeProm(struct args *arg, char promName[], int pos){
    int k=0;
    for(int i =0; i< arg->nPromotores;i++){
        if(strcmp(promName,arg->p[i].promName)==0){
            k=i;
            int j=pos+1;
            while(j<=arg->p[i].nPromos){
                strcpy(arg->p[i].promo[j-1], arg->p[i].promo[j]);
                j++;
            }
        }
    }
    arg->p[k].nPromos--;

}

void removeClient(struct args *arg, pid_t pid_remove){

    for(int i=0; i<arg->login.nUsers;i++)
    {
        if(pid_remove==arg->login.usersLogin[i].userPid)
        {
            printf("O cliente %s desapareceu\n", arg->login.usersLogin[i].username);
            int pos=i;
            i= pos+1;
            while(i<=arg->login.nUsers-1)
            {
                arg->login.usersLogin[i-1] = arg->login.usersLogin[i];
                i++;
            }
        }
    }
    for(int j =0; j<arg->login.nUsers;j++){
        if(pid_remove==arg->heartbeat_back[j].pid)
        {
            int pos=j;
            j= pos+1;
            while(j<=arg->login.nUsers-1)
            {
                arg->heartbeat_back[j-1] = arg->heartbeat_back[j];
                j++;
            }
        }
    }
    arg->login.nUsers--;

}


void *checkHeartbeats(void *l){
    struct args *arg = (struct args * ) l;
    printf("Entrei na thread dos heartbeats\n");
    do{
        pid_t pid_remove = 0;
        for(int i =0; i<arg->login.nUsers;i++){
            time_t last = arg->heartbeat_back[i].time1;
            time_t current = time(NULL);
            if(difftime(current, last)>arg->nSecHeart)
                pid_remove = arg->heartbeat_back[i].pid;
        }
        if(pid_remove!=0) {removeClient(arg, pid_remove);}
    }while(arg->ativo!=1);
    pthread_exit(NULL);

}

void updateFileItems(struct item lista_items[], int n){
    char *fileName = getenv("FITEMS");
    fclose(fopen(fileName, "w"));
    FILE *f = fopen(fileName, "w");

    for(int i=0; i< n;i++){
        fprintf(f,"%d %s %s %d %d %d %s %s\n", lista_items[i].id, lista_items[i].nome, lista_items[i].categoria, lista_items[i].valorBase, lista_items[i].comprarJa, lista_items[i].durLeilao, lista_items[i].userVende, lista_items[i].userCompra);
    }
    fclose(f);
}

void *verificaItems(void *l){
    printf("Entrei na thread p verificar os items\n");
    char msg[500];
     struct args *arg = (struct args * ) l;
     int d=0;
    pthread_mutex_lock(arg->m);

    do{
        d=0;
        for(int i=0; i< arg->nItems;i++){
            arg->lista[i].durLeilao--;
            if(arg->lista[i].durLeilao<=0) {
                d=1;
                if(strcmp(arg->lista[i].userCompra, "-")==0) {
                    sprintf(msg,
                            "O produto com o id %d, com o nome %s, e de categoria %s não foi vendido por %d e o leilão terminou",
                            arg->lista[i].id, arg->lista[i].nome, arg->lista[i].categoria, arg->lista[i].valorBase);
                }
                else{
                    sprintf(msg,
                            "O produto com o id %d, com o nome %s, e de categoria %s foi vendido por %d ao utilizador %s e o leilão terminou",
                            arg->lista[i].id, arg->lista[i].nome, arg->lista[i].categoria, arg->lista[i].valorBase,
                            arg->lista[i].userCompra);
                    updateUserBalance(arg->lista[i].userCompra, getUserBalance(arg->lista[i].userCompra)-arg->lista[i].valorBase);
                    updateUserBalance(arg->lista[i].userVende, getUserBalance(arg->lista[i].userVende)+arg->lista[i].valorBase);
                }
                sendNot(arg, msg);
                int pos=i;
                int j = pos+1;
                while(j<=arg->nItems-1)
                {
                    arg->lista[j-1] = arg->lista[j];
                    j++;
                }
            }
            pthread_mutex_unlock(arg->m);
        }
        if(d==1)
            arg->nItems--;

        sleep(1);
    }while(arg->ativo!=1);
    pthread_exit(NULL);
}

int verificaLicitacao(struct args *arg, int id, int valor, char username[]){
    char msg[500];
    for(int i=0; i< arg->nItems;i++){
        if(arg->lista[i].id == id){
            if(arg->lista[i].comprarJa!=0){
                if (valor >= arg->lista[i].comprarJa) {
                    sprintf(msg, "O utilizador %s adquiriu o produto: %d %s %s por %d, parabéns",username, arg->lista[i].id, arg->lista[i].nome, arg->lista[i].categoria, arg->lista[i].comprarJa);
                    updateUserBalance(username, getUserBalance(username)-valor);
                    updateUserBalance(arg->lista[i].userVende, getUserBalance(arg->lista[i].userVende)+arg->lista[i].comprarJa);
                    sendNot(arg, msg);
                    return 0; //fica com o produto
                }
            }
            if(valor <= arg->lista[i].valorBase)
                return 1; //Licitacao invalida
            else if(valor > arg->lista[i].valorBase){
                strcpy(arg->lista[i].userCompra, username);
                arg->lista[i].valorBase = valor;
                sprintf(msg, "O utilizador %s licitou o produto: %d %s %s por %d",username, arg->lista[i].id, arg->lista[i].nome, arg->lista[i].categoria, arg->lista[i].comprarJa);
                sendNot(arg, msg);
                return 2;
            }
        }
    }
    return 1;
}

void *login(void *l){
    printf("Entrei na thread do login\n");
    struct args *arg = (struct args * ) l;
    char nome_fifo_FE[100];
    char msg[200];
    struct user user1;
    char delim[]=" ", *aux;
    int resp = 1;
    int fd = open(BE_FIFO,O_RDWR);
    do{
        /*mostra_info(arg->lista, arg->nItems);
        sleep(5);*/
        resp = 1;
        int size = read(fd,&user1,sizeof(user1));
        if(size>0) {
            //envio da resposta ao frontend
            sprintf(nome_fifo_FE, FE_FIFO, user1.userPid);
            if(user1.tipo==0) {
                // 0- registado; 1 - nao existe; 2 - ja esta logado
                if (isUserValid(user1.username, user1.password)) {
                    if (arg->login.nUsers == MAXUSERS) resp = 1;
                    for (int i = 0; i < arg->login.nUsers; i++) {
                        if (strcmp(arg->login.usersLogin[i].username, user1.username) == 0)
                            resp = 2;
                    }
                    if (resp != 2) {
                        strcpy(arg->login.usersLogin[arg->login.nUsers].username, user1.username);
                        strcpy(arg->login.usersLogin[arg->login.nUsers].password, user1.password);
                        arg->login.usersLogin[arg->login.nUsers].userPid = user1.userPid;
                        arg->heartbeat_back[arg->login.nUsers].pid = user1.userPid;
                        arg->heartbeat_back[arg->login.nUsers].time1 = time(NULL);
                        arg->login.nUsers++;
                        printf("Nusers: %d\n", arg->login.nUsers);
                        resp = 0;
                    }
                }
                int fdEnvio = open(nome_fifo_FE, O_WRONLY);
                int s2 = write(fdEnvio, &resp, sizeof(resp));
                close(fdEnvio);
            } //login
            else if(user1.tipo == 1){
                aux = strtok(user1.comando, delim);
                if(strstr("cash\n",aux)){
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                            int userBalance= getUserBalance(arg->login.usersLogin[i].username);
                            sprintf(user1.resposta, "Saldo: %d", userBalance);
                        }
                    }
                }
                else if(strstr("add", aux)){
                    int cash = atoi(strtok(NULL, delim));
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                            updateUserBalance(arg->login.usersLogin[i].username, getUserBalance(arg->login.usersLogin[i].username)+cash);
                            strcpy(user1.resposta, "Saldo Atualizado");
                            saveUsersFile(getenv("FUSERS"));
                        }
                    }
                }
                else if(strstr("list\n", aux)){
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                             strcpy(user1.resposta, lista_cliente(arg->lista, 0, "", arg->nItems));
                        }
                    }
                }
                else if(strstr("licat", aux)){
                    char *cat = strtok(NULL, delim);
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                            strcpy(user1.resposta, lista_cliente(arg->lista, 1, cat, arg->nItems));
                        }
                    }
                }
                else if(strstr("lisel", aux)){
                    char *vend = strtok(NULL, delim);
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                            strcpy(user1.resposta, lista_cliente(arg->lista, 2, vend, arg->nItems));
                        }
                    }
                }
                else if(strstr("lival", aux)){
                    char *val = strtok(NULL, delim);
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                            strcpy(user1.resposta, lista_cliente(arg->lista, 3, val, arg->nItems));
                        }
                    }
                }
                else if(strstr("litime", aux)){
                    char *dur = strtok(NULL, delim);
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                            strcpy(user1.resposta, lista_cliente(arg->lista, 4, dur, arg->nItems));
                        }
                    }
                }
                else if(strstr("sell", aux)){
                    char *nome = strtok(NULL, delim);
                    char *cat = strtok(NULL, delim);
                    int pBase = atoi(strtok(NULL, delim));
                    int pCompreJa = atoi(strtok(NULL, delim));
                    int dur = atoi(strtok(NULL, delim));
                    int id=0;
                    while(1){
                        id = rand() % 1000 + 1;
                        if(verifica_id(arg->lista, id, arg->nItems)==0)
                            break;
                    }
                    for(int i=0; i<arg->login.nUsers; i++){
                        if(arg->login.usersLogin[i].userPid==user1.userPid){
                            int n = arg->nItems;
                            arg->lista[n].id=id;
                            strcpy(arg->lista[n].nome,nome);
                            strcpy(arg->lista[n].categoria,cat);
                            arg->lista[n].valorBase=pBase;
                            arg->lista[n].comprarJa=pCompreJa;
                            arg->lista[n].durLeilao=dur;
                            strcpy(arg->lista[n].userVende,user1.username);
                            strcpy(arg->lista[n].userCompra,"-");
                            arg->nItems++;
                            mostra_info(arg->lista, arg->nItems);
                            sprintf(user1.resposta, "Produto a venda com o id %d", id);
                        }
                    }
                    sprintf(msg, "Novo produto a venda: %d %s %s %d %d %d %s", id, nome, cat, pBase, pCompreJa, dur, user1.username);
                    sendNot(arg, msg);
                }
                else if(strstr("buy", aux)){
                    int id = atoi(strtok(NULL, delim));
                    int lic = atoi(strtok(NULL, delim));
                    if(verifica_id(arg->lista, id, arg->nItems)==0) {
                        sprintf(user1.resposta, "O id %d não existe", id);
                    }
                    else{
                        if(getUserBalance(user1.username)<=0 || getUserBalance(user1.username)<lic) {
                            sprintf(user1.resposta, "Não tem saldo disponível");
                        }else {
                            int v = verificaLicitacao(arg, id, lic, user1.username);
                            if(v==0)
                                sprintf(user1.resposta, "Adquiriu o produto %d, parabéns", id);
                            else if(v==1)
                                sprintf(user1.resposta, "Valor inferior ao pedido, ignorado");
                            else if(v==2)
                                sprintf(user1.resposta, "Licitação feita");
                        }
                    }

                }
                int fdEnvio = open(nome_fifo_FE, O_WRONLY);
                int s2 = write(fdEnvio, &user1, sizeof(user1));
                close(fdEnvio);
            } //comandos
            else if(user1.tipo ==2){
                for(int i =0; i<arg->login.nUsers; i++){
                    if(arg->heartbeat_back[i].pid==user1.userPid) {
                        printf("Recebi heartbeat do cliente %s\n", arg->login.usersLogin[i].username);
                        arg->heartbeat_back[i].time1 = time(NULL);
                    }
                }
            } //heartbeat
            else if(user1.tipo==3){
                removeClient(arg, user1.userPid);
            } //o cliente informa a sua saída
        }
    }while(arg->ativo!=1);
    close(fd);
    pthread_exit(NULL);
}

void *checkProms(void *l){
    char msg[500];
    int lugar=0,i,j;
    char delim[]=" ",nome[100];
    struct args *arg = (struct args * ) l;
    for(i=0; i< arg->nPromotores; i++){
        if(arg->p[i].checkItems==0){
            lugar=i;
            arg->p[i].checkItems=1;
            strcpy(nome,arg->p[i].promName);
            break;
        }
    }
    printf("A verificar promoções do %s\n",nome);
    do{
        pthread_mutex_lock(arg->m);
        for(j=0; j<arg->p[lugar].nPromos;j++){
            char *aux = strtok(arg->p[lugar].promo[j], delim);
            int cash = atoi(strtok(NULL, delim));
            int dur = atoi(strtok(NULL, delim));
            dur--;
            if(dur==0){

                printf("Acabou:[%s]",arg->p[lugar].promo[j]);
                sprintf(msg, "Acabou a promoção dos artigos: %s", aux);
                sendNot(arg, msg);
                removeProm(arg,nome,j);

            }else {
                printf("%s NUMPROM:[%d]\n", nome, arg->p[lugar].nPromos);
                printf("%s :%s %d %d\n",nome, aux, cash, dur);
                sprintf(arg->p[lugar].promo[j], "%s %d %d", aux, cash, dur);
            }

        }
        pthread_mutex_unlock(arg->m);
        sleep(1);
    }while(arg->ativo!=1);
    pthread_exit(NULL);
}

void *prom(void *l){
    struct args *arg = (struct args * ) l;
    int fd_rec[2], lugar=0;
    char str[100], delim[]=" ",nome[100], *aux, msg[500];
    for(int i=0; i< arg->nPromotores; i++){
        if(arg->p[i].ativo==0){
            lugar=i;
            arg->p[i].ativo=1;
            strcpy(nome,arg->p[i].promName);
            break;
        }
    }
    printf("Entrei na thread do prom %s\n", nome);
    pipe(fd_rec);

        pid_t pid = fork();
        arg->p[lugar].pid = pid;

        if (pid == 0) {
            close(STDOUT_FILENO);
            dup(fd_rec[1]);
            close(fd_rec[1]);
            close(fd_rec[0]);
            execlp(arg->p[lugar].promName,arg->p[lugar].promName, NULL);

            perror("Prom não encontrado\n");
            close(fd_rec[0]);
            close(fd_rec[1]);
            pthread_exit(NULL);
        } else if (pid > 0) {
            fflush(stdout);
            close(fd_rec[1]);
            do{
                int sizeP = read(fd_rec[0], str, sizeof(str) - 1);
                if(sizeP>1) {
                    str[sizeP] = '\0';
                    printf("%s: %s\n", arg->p[lugar].promName, str);
                    strcpy(arg->p[lugar].promo[arg->p[lugar].nPromos], str);


                    arg->p[lugar].nPromos++;
                    pthread_mutex_lock(arg->m);
                    char *aux1 = strtok(str, delim);
                    int cash = atoi(strtok(NULL, delim));
                    for(int k=0; k<arg->nItems; k++){
                        if(strcmp(aux1, arg->lista[k].categoria)==0 && arg->lista[k].emPromocao==0){
                            arg->lista[k].emPromocao=1;
                            //printf("Cash: %d\n", cash);
                            float desconto = cash/100;
                            //printf("Desconto: %0.2f\n", desconto);
                            int valorBase = arg->lista[k].valorBase;
                            //printf("Valor Base %d\n", valorBase);
                            float valorDesconto = valorBase*desconto;
                            //printf("Valor Desconto %f\n", valorDesconto);
                            arg->lista[k].valorBase = valorBase-valorDesconto;
                            sprintf(msg, "O artigo %s está em promoção com o preço %d", arg->lista[k].nome, arg->lista[k].valorBase);
                            sendNot(arg, msg);
                        }
                     }
                    pthread_mutex_unlock(arg->m);
                }
            }while(arg->ativo!=1);
        }

    pthread_exit(NULL);
}

void termina(int s,siginfo_t *t, void *p){
    
}

int main(int argc, char **argv){
    if(argc!=1){
        printf("Wrong Number of Arguments\n");
        exit(0);}

    struct sigaction sa1;
    sa1.sa_sigaction = termina;
    sa1.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sa1, NULL);

    char *fusers, *fprom,*fitems,*seconds, *aux, *secondWord, delim[]=" ", command[100]="",aux_char[100],firstProm[30];;
    int t=0,exit =0, temp=0, nUsers=0, i=0, pt=0, line =0, stat;
    char arrProm[MAXPROM][MAXLEN];
    union sigval val;
    struct args arg;
    pthread_mutex_t  mutex;
    pthread_mutex_init(&mutex, NULL); //inicializar
    arg.ativo=0;
    arg.login.nUsers=0;
    arg.nItems=0;
    arg.nPromotores=0;

    setbuf(stdout,NULL);
    //Variaveis de Ambiente
    fusers = getenv("FUSERS");
    if(fusers==NULL) erroVariavel("FUSERS");
    if(access(fusers, F_OK)!=0) fileNotExists(fusers);

    nUsers = loadUsersFile(fusers);

    fprom = getenv("FPROMOTERS");
    if(fprom==NULL) erroVariavel("FPROMOTERS");
    if(access(fprom, F_OK)!=0) fileNotExists(fusers);
    else{
        FILE *f = fopen(fprom, "r");
            while(fgets(arrProm[line], MAXLEN, f)!=NULL) {
                arrProm[line][strcspn(arrProm[line],"\n")] = '\0';
                line++;
            }
        fclose(f);
    }

    seconds = getenv("HEARTBEAT");
    if(seconds==NULL) erroVariavel("HEARTBEAT");

    pthread_t valid_cred, hearts, updateItems;
    pthread_t ouve_prom[line], promItems[line];
    for(i =0; i<line; i++) {
        strcpy(arg.p[i].promName, arrProm[i]);
        printf("%d: %s\n",i, arg.p[i].promName);
    }
    fitems = getenv("FITEMS");
    if(fitems!=NULL) {
        if(access(fitems, F_OK)!=0) fileNotExists(fusers);
        FILE *f = fopen(fitems, "r");
        int id, comprarJa, valorBase, durLeilao, lido;
        char nome[100], categoria[100], userVende[100], userCompra[100];
        i=0;
        while(fscanf(f, "%d %s %s %d %d %d %s %s", &id, nome, categoria, &valorBase,&comprarJa,&durLeilao, userVende, userCompra)!=EOF && i<MAXITEMS) {
            arg.lista[i].id=id;
            strcpy(arg.lista[i].nome,nome);
            strcpy(arg.lista[i].categoria,categoria);
            arg.lista[i].valorBase=valorBase;
            arg.lista[i].comprarJa=comprarJa;
            arg.lista[i].durLeilao=durLeilao;
            strcpy(arg.lista[i].userVende,userVende);
            strcpy(arg.lista[i].userCompra,userCompra);
            arg.lista[i].emPromocao=0;
            i++;
        }
        arg.nItems = i;
        fclose(f);
    }

    arg.m = &mutex;

    if(mkfifo(BE_FIFO,0666)==-1){
        if(errno ==EEXIST) {
        printf("Servidor em execucao ou fifo ja existe\n");
        return 1;
    }
        printf("Erro a abrir fifo");
        return 1;
    }
    arg.nSecHeart = atoi(seconds);

    if(pthread_create(&hearts, NULL, &checkHeartbeats, &arg)!=0)
        return -1;
    if(pthread_create(&valid_cred, NULL, &login, &arg)!=0)
        return -1;
    if(pthread_create(&updateItems, NULL, &verificaItems, &arg)!=0)
        return -1;
    for(i=0; i<line;i++) {
        arg.p[i].ativo=0;
        arg.p[i].checkItems=0;
        arg.p[i].nPromos=0;

        if (pthread_create(&ouve_prom[i], NULL, &prom, &arg) != 0)
            return -1;
        if (pthread_create(&promItems[i], NULL, &checkProms, &arg) != 0)
            return -1;
        arg.nPromotores++;
    }


    printf("Pronto para receber comandos do administrador\n");
    while(exit!=1){
        fgets(command, sizeof(command), stdin);
        secondWord = NULL;
        aux = strtok(command, delim);
        if (strstr(aux, "users")) {
            if(arg.login.nUsers==0) printf("Não existem utilizadores logados\n");
            for(i=1; i<=arg.login.nUsers;i++){
                printf("%s\n", arg.login.usersLogin[i-1].username);
            }
        } //está
        else if (strstr(aux, "list")) {
            mostra_info(arg.lista, arg.nItems);
        } //está
        else if (strstr(aux, "reprom")) {
            printf("Reprom\n");
/*            int k=0, indice=0;
            FILE *f = fopen(fprom, "r");
            while(fgets(arrProm[line], MAXLEN, f)!=NULL) {
                arrProm[line][strcspn(arrProm[line],"\n")] = '\0';
                line++;

            }
            line-=2;
            fclose(f);

            for(int j=0;j<arg.nPromotores;j++){
                k=0;
                indice=0;
            for(i =0; i<line; i++) {
                indice=j;
                if(strcmp(arg.p[j].promName, arrProm[i])==0){
                        k=1;
                        break;
                    }
                 }
                if(k==0){
                   removePromotor(&arg, arg.p[indice].promName);
                   sigqueue(arg.p[indice].pid, SIGUSR1, val);
                   arg.p[i].ativo = 1;
                   printf("A fechar promotor\n");
                   printf("Pid: %d\n", arg.p[i].pid);
                   if (pthread_join(ouve_prom[indice], NULL) != 0) return -1;
                   if (pthread_kill(ouve_prom[indice], SIGUSR2) != 0) return -1;
                }
            }
            
        for(int j=0;j<arg.nPromotores;j++){
                k=0;
                indice=0;
           for(i =0; i<line; i++) {
                indice=j;
              if(strcmp(arg.p[i].promName, arrProm[j])!=0){
                    printf("nome prom: [%s]",arg.p[j].promName);
                    k=1;
                    break;
            }
            }
            if(k==0){
                    strcpy(arg.p[indice].promName,arrProm[indice]);
                    arg.p[indice].ativo=0;
                    arg.p[indice].checkItems=0;
                    arg.p[indice].nPromos=0;

        if (pthread_create(&ouve_prom[indice], NULL, &prom, &arg) != 0)
            return -1;
        if (pthread_create(&promItems[indice], NULL, &checkProms, &arg) != 0)
            return -1;
        arg.nPromotores++;
                    }
}
*/
}

        
        else if (strstr(aux, "prom")) {
            for(i=0; i<line;i++) {
                if(arg.p[i].ativo==1) printf("%s\n", arg.p[i].promName);
            }
        } //está
        else if (strstr(aux, "cancel")) {
            secondWord = strtok(NULL, delim);
            char promotor[100] = "./";
            if (secondWord == NULL) printf("Comando Inválido\n");
printf("Cancel\n");
/*            else {
                secondWord[strcspn(secondWord, "\n")] = '\0';
                strcat(promotor, secondWord);
                for (i = 0; i < line; i++) {
                    if (arg.p[i].ativo == 0 && strcmp(promotor, arg.p[i].promName) == 0) {

                        if (pthread_kill(ouve_prom[i], SIGUSR2) != 0)
                            return -1;
                        if (pthread_kill(promItems[i], SIGUSR2) != 0)
                            return -1;
                        if (pthread_join(ouve_prom[i], NULL) != 0)
                            return -1;
                        if (pthread_join(promItems[i], NULL) != 0)
                            return -1;
                        sigqueue(arg.p[i].pid, SIGUSR1, val);

                        printf("A fechar promotor\n");
                        printf("Pid: %d\n", arg.p[i].pid);
                        removePromotor(&arg, arg.p[i].promName);
                        printf("Promotor %s cancelado\n", promotor);
                        kill(arg.p[i].pid, SIGINT);
                        arg.p[i].ativo = 1;
                    }
                }
            }
*/

        }
        else if (strstr(aux, "kick")) {
            secondWord = strtok(NULL, delim);
            if (secondWord == NULL) printf("Comando Inválido\n");
            else{
                secondWord[strcspn(secondWord, "\n")] = '\0';
                if(arg.login.nUsers==0) printf("Não existem utilizadores logados\n");
                for(i=1; i<=arg.login.nUsers;i++) {
                    if(strcmp(arg.login.usersLogin[i-1].username, secondWord)==0) {
                        printf("Name: %s\nPid: %d", arg.login.usersLogin[i-1].username,arg.login.usersLogin[i-1].userPid);
                        sigqueue(arg.login.usersLogin[i-1].userPid, SIGINT, val);
                    }
                }
            }
        } //está
        else if (strstr(aux, "close")) {
            updateFileItems(arg.lista, arg.nItems);
            for(i=1; i<=arg.login.nUsers;i++) {
                    printf("Name: %s\nPid: %d\n", arg.login.usersLogin[i-1].username,arg.login.usersLogin[i-1].userPid);
                    sigqueue(arg.login.usersLogin[i-1].userPid, SIGINT, val);
            }
            exit = 1;
        } //está
        else printf("Comando Inválido\n");
    }

    arg.ativo=1;
    saveUsersFile(getenv("FUSERS"));

    for(i=0; i<line;i++) {
        arg.p[i].ativo=1;
        if (pthread_kill(ouve_prom[i], SIGUSR2) != 0)
            return -1;
        if (pthread_kill(promItems[i], SIGUSR2) != 0)
            return -1;
    }
    if (pthread_kill(valid_cred, SIGUSR2) != 0)
        return -1;

    if (pthread_kill(hearts, SIGUSR2) != 0)
        return -1;
    if (pthread_kill(updateItems, SIGUSR2) != 0)
        return -1;

    pthread_mutex_destroy(&mutex);
    for(i=0; i<line;i++) {
        sigqueue(arg.p[i].pid, SIGUSR1, val);
        arg.p[i].ativo = 1;
        printf("A fechar promotores\n");
        printf("Pid: %d\n", arg.p[i].pid);
    }

    for(i=0; i<line;i++) {
        if (pthread_join(ouve_prom[i], NULL) != 0)
            return -1;
        if (pthread_join(promItems[i], NULL) != 0)
            return -1;
    }

    if(pthread_join(valid_cred, NULL)!=0)
        return -1;
    if(pthread_join(hearts, NULL)!=0)
        return -1;
    if(pthread_join(updateItems, NULL)!=0)
        return -1;
    unlink(BE_FIFO);
    return 0;
}
