//
// Created by valyo95 on 4/6/2017.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/stat.h>
#include "mirrorServer.h"



int total_contIds;
pthread_mutex_t  contIdMutex=
        PTHREAD_MUTEX_INITIALIZER;


int isDirMine(char * dir, char *myDir, char *cwd)
{
    char *tmp = dir;
    int lenCwd = strlen(cwd)+1;
    tmp+=lenCwd*sizeof(char);
//    printf("tmp: %s\n", tmp);
    int len = strlen(myDir);
    char *myTemp = myDir;
    int k = 0;
    int l = 0;
    while(tmp[k] != '\0')
    {
        if(tmp[k] == myTemp[l])
        {
            l++;
            k++;
        }
        else
            return  0;

        if(l == len)
            if(tmp[k] == '/' || tmp[k] == '\0')
            {
//                tmp+=k*sizeof(char);
//                printf("%s\n", (tmp));
                return 1;
            }
            else
                l = 0;
    }
    return 0;
}

void *mirrorManager(void * argp)
{
    char *buff = malloc(BUFFSIZE*sizeof(char));
    int sock;
    struct serverDir *servP = (struct serverDir *)argp;
    struct sockaddr_in server;
    server.sin_addr = servP->server.sin_addr;
    server.sin_family = AF_INET;
    server.sin_port = servP->server.sin_port;
    struct sockaddr * serverptr = (struct sockaddr *) &server;
    struct hostnet *rem;


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock <0)
        perror_exit("socket");
    if(connect(sock, serverptr, sizeof(server)) < 0)
        perror_exit("connect to content server");

    printServerDir(servP);

    int msg = LIST;
    if(write(sock, &msg, sizeof(int)) != sizeof(int))
        perror_exit("write LIST");



    pthread_mutex_lock(&contIdMutex);
    servP->contentId = total_contIds;
    total_contIds++;
    pthread_mutex_unlock(&contIdMutex);

    printServerDir(servP);

    if(write(sock, servP, sizeof(struct serverDir)) != sizeof(struct serverDir))
        perror_exit("write serrverDirP");

    char cntCwd[BUFFSIZE];
    int type;
    read(sock, &cntCwd, BUFFSIZE);
    while(read(sock,&type, sizeof(int)) > 0)
    {
        read(sock,buff, BUFFSIZE);

        if(isDirMine(buff, servP->dirName, cntCwd))
        {
            int len = strlen(cntCwd)+1;
            char *tmpDir = buff;
            tmpDir+=len*sizeof(char);

            if(type == DIR)
            {
                printf("is dir: %s\n", tmpDir);
                if(mkdir(tmpDir, 0777) != 0)
                    perror_exit("mkdir");            }
            else if(type == FFILE)
            {
                printf("is file: %s\n", tmpDir);
            }

//            printf("%s", buff);
//            strcpy(servP->dirName, buff);
//            printServerDir(servP);
        }
    }

    printf("Connected!\n");

    return (void *) 0;
}

int main(int argc, char **argv)
{
    total_contIds = 0;
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;

    int newsock;
    int connections;

    int sock;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *) &server;
    struct sockaddr *clientptr = (struct sockaddr *) &client;
    struct hostent *rem;
    pthread_t *thr;

    int i = 0;
    char *dirname = NULL;
    int threadNum = -1;

    if(argc != 7) {
        fprintf(stderr, "Wrong argument.\n Usage: ./MirrorServer -p"
                "<port> -m < dirname > -w <threadnum>\n");
    }
    while (i < argc) {
        if(strcmp(argv[i], "-p") == 0) {
            server.sin_port = htons((uint16_t)atoi(argv[i + 1]));
        } else if(strcmp(argv[i], "-m") == 0) {
            dirname = strdup(argv[i + 1]);
        } else if(strcmp(argv[i], "-w") == 0) {
            threadNum = atoi(argv[i + 1]);
            if(threadNum <=0)
            {
                fprintf(stderr, "Error argument. threadnum muust be >=1\n");
                exit(1);
            }
        }
        i++;
    }
    if(mkdir(dirname, 0777) != 0)
        perror_exit("mkdir");
    chdir(dirname);
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        perror("getcwd() error");
    printf("CWD: %s\n", cwd);

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    if(bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    if(listen(sock, 5) < 0) perror_exit("listen");

    printf("Listening for connections to port %d\n", ntohs(server.sin_port));
    while (1) {
        clientlen = sizeof(client);
        if((newsock = accept(sock, clientptr, &clientlen))
           < 0) {
            perror_exit("accept");
        }

        if((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) ==
           NULL) {
            herror("gethostbyaddr");
            exit(1);
        }
        printf("Accepted connection from %s\n", rem->h_name);
        if(read(newsock, &connections, sizeof(int)) != sizeof(int))
            perror_exit("reading number of connections");
        printf("I'm gonna have %d connections\n", connections);

        serverDir ** pServerDir = malloc(connections*sizeof(struct serverDir *));
        thr = malloc(connections*sizeof(pthread_t));

        for (i = 0; i < connections; ++i) {
            pServerDir[i] = malloc(1*sizeof(struct serverDir));
            int rd = read(newsock, pServerDir[i], sizeof(struct serverDir));
            printf("rd = %d sizeof(serverId) = %d\n", rd, sizeof(struct serverDir));
            if(rd < 0)
                perror_exit("reading the connetions");

            pthread_create(&(thr[i]), NULL, mirrorManager, (void *) pServerDir[i]);

        }
    }
    return  0;
}