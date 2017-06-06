//
// Created by valyo95 on 4/6/2017.
//

#ifndef SYSPRO3_MIRRORMANAGER_H_H
#define SYSPRO3_MIRRORMANAGER_H_H

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define  BUFFSIZE 2000
#define MAXWORDS 300

#define  DIRNAMESIZE 2000
#define  LIST 1234
#define  FETCH 5678

#define DIR 5555
#define FFILE 7777


typedef  struct serverDir{
    struct sockaddr_in server;
    char dirName[DIRNAMESIZE];
    int delay;
    int contentId;
}serverDir;

serverDir * createServerDir(struct sockaddr_in server, char * dirName, int delay)
{
    serverDir * ser = malloc(1*(sizeof(struct serverDir)));
    ser->server = server;
    ser->server.sin_family = AF_INET;
    strcpy(ser->dirName, (dirName));
    ser->delay = delay;
    return  ser;
}

void deleteServerDir(serverDir * server)
{
    free(server);
    return;
}

void printServerDir(serverDir * server)
{
    int len=20;
    char buffer[len];

    inet_ntop(AF_INET, &(server->server.sin_addr), buffer, len);
    printf("Address: %s\n",buffer);
    printf("Port: %d\nDelay: %d\nDirname: %s\nContentId: %d\n\n",  ntohs(server->server.sin_port), server->delay, server->dirName, server->contentId);
}

void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

typedef struct serverList
{
    serverDir *server;
    struct serverList *next;
}serverList;

serverList* createServerList()
{

}

#endif //SYSPRO3_MIRRORMANAGER_H_H
