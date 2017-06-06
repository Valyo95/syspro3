//
// Created by valyo95 on 4/6/2017.
//


#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <unistd.h>
#include "mirrorServer.h"


int cutLine(char *line, char **word, char *delimit)
{
    int i = 0;
    char *tmp = NULL;
    tmp = strdup(line);
    char *tmp1 = tmp;

    char *token;
    int k = 0;
    k = (int) strlen(tmp);

    while( (token = strsep(&tmp,delimit)) != NULL )
    {
        word[i] = strdup(token);
        k-=strlen(word[i])+1;
        while(k > 0 && isspace(tmp[0]))
        {
            tmp+=sizeof(char);
            k--;
        }
        i++;
    }
    word[i] = NULL;
    free(tmp1);

    for(k = 0; k < i; k++)
        printf("%d: %s\n", k, word[k]);

    return i;
}


serverDir **getFromString(char * str, int *connects)
{
    char *tmp = str;
    char **word = malloc(200 * sizeof(char *));

    int words = cutLine(tmp, word, ",");

    *connects = words;
//    printf("I have %d connections.\n", connects);
    tmp = str;
    int j;
    serverDir **pServerDir = malloc(words * sizeof(struct serverDir *));

    char **word1 = malloc(200 * sizeof(char *));
    for (j = 0; j < words; ++j) {
        int words1 = cutLine(word[j], word1, ":");

        pServerDir[j] = malloc(1 * sizeof(struct serverDir));
        struct hostent *he;
        int port = atoi(word1[1]);
        pServerDir[j]->server.sin_port = htons((uint16_t) port);

        if ( (he = gethostbyname(word1[0]) ) == NULL ) {
            perror("gethostbyname(): ");
            exit(1); /* error */
        }

        memcpy(&(pServerDir[j]->server.sin_addr), he->h_addr_list[0], he->h_length);
        strcpy(pServerDir[j]->dirName, (word1[2]));
        pServerDir[j]->delay = atoi(word1[3]);
        pServerDir[j]->contentId = -1;
        int k;
        for (k = 0; k < words1; k++) {
            free(word1[k]);
        }
    }

    for (j = 0; j < words; ++j) {
        free(word[j]);
    }


    free(word);
    free(word1);

    return pServerDir;
}

int main(int argc, char **argv)
{
    struct hostent *he;
    int sock;
    int i = 0;
    struct sockaddr_in server;
    server.sin_family = AF_INET;

    int connections = -1;
    int serverPort = -1;
    serverDir ** pServerDir;
    char *serverName = NULL;
    int threadNum = -1;
    while (i < argc) {
        if(strcmp(argv[i], "-p") == 0) {
            server.sin_port = htons((uint16_t)atoi(argv[i + 1]));
        } else if(strcmp(argv[i], "-n") == 0) {
            if ( (he = gethostbyname(argv[i+1]) ) == NULL ) {
                perror("gethostbyname(): ");
                exit(1); /* error */
            }
            memcpy(&(server.sin_addr), he->h_addr_list[0], he->h_length);

        } else if(strcmp(argv[i], "-s") == 0) {
            pServerDir = getFromString(argv[i + 1], &connections);
        }
        i++;
    }
    // Print the connections we want
    for (int j = 0; j < connections; ++j) {
        printServerDir(pServerDir[j]);
    }


    //
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    if (connect(sock,  (struct sockaddr*)&server, sizeof(server)) < 0)
        perror_exit("connect");
    printf("Connecting to %s port %d\n", argv[1], ntohs(server.sin_port));

    if(write(sock, &connections, sizeof(int)) != sizeof(int))
        perror_exit("writing number of conenctions: ");

    for (i = 0; i < connections; ++i) {
       if( write(sock, pServerDir[i], sizeof(struct serverDir)) !=sizeof(struct serverDir))
           perror_exit("writing the connetions");
    }

    for (int i = 0; i < connections; ++i) {
        deleteServerDir((pServerDir[i]));
    }
    free(pServerDir);

    free(serverName);
    return 0;
}