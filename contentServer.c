//
// Created by valyo95 on 27/5/2017.
//

#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <bits/signum.h>
#include "mirrorServer.h"

char *myDir = NULL;

int cutLine(char *line, char **word);

void *child_server(void* argp) {

    char buf[1];
    int *newsockp = (int *) argp;
    int newsock =  *newsockp;
    printf("I'm the new thread with newsock = %d\n", newsock);

    char *buff = malloc(BUFFSIZE*sizeof(char));

    int i = 0;
    int message;
    if(read(newsock, &message, sizeof(int)) != sizeof(int))
        //perror_exit("read Message");
        printf("Message: %d\n", message);
    if(message == LIST)
    {
        printf("gonna LIST now\n");
        struct serverDir *serverDirP = malloc(1*sizeof(struct serverDir));
        if(read(newsock, serverDirP, sizeof(struct serverDir)) != sizeof(struct serverDir))
            perror_exit("read ServerDirP");

/*
        printf("readed:\n");
        printServerDir(serverDirP);
*/

        char cwd[BUFFSIZE];

        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("getcwd() error");
        if(write(newsock, cwd, BUFFSIZE) != BUFFSIZE)
            perror_exit("write cwd");

        //Sendind the directories names
        FILE *pop = NULL;
        char command[BUFFSIZE];
        strcpy(command, "find $PWD -type d ");
        if((pop = popen(command, "r")) == NULL)
            perror_exit("popen pop");

        int type = DIR;

        while(fgets(buff, BUFFSIZE, pop)!=NULL){

            buff[strlen(buff)-1] = '\0';
            printf("%s\n", buff);

            write(newsock, &type, sizeof(int));
            write(newsock, buff, BUFFSIZE);
        }
        pclose(pop);

        //Sendind the filenames
        pop = NULL;
        strcpy(command, "find $PWD -type f ");
        if((pop = popen(command, "r")) == NULL)
            perror_exit("popen pop");

        type = FFILE;

        while(fgets(buff, BUFFSIZE, pop)!=NULL){

            buff[strlen(buff)-1] = '\0';
            printf("%s\n", buff);

            write(newsock, &type, sizeof(int));
            write(newsock, buff, BUFFSIZE);
        }
        pclose(pop);

    }
    else if(message == FETCH)
    {
        printf("gonna FETCH now\n");
    }
    else
    {
        fprintf(stderr, "Error message.\n");
        return (void *)-1;
    }
    buff[i] = '\0';
    i++;
    printf("buffer: %s\n", buff);

    printf("Closing connection.\n");
//    close(newsock);	  /* Close socket */

/*
    for (int j = 0; j < words; ++j) {
        free(word[j]);
    }
    free(word);
    free(buff);
*/
}

void perror_exit(char *msg);
void sanitize(char *str);

int main(int argc, char *argv[]) {
    int             port, sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent *rem;
    char * dirname;
    int i = 1;
    if(argc != 5){
        fprintf(stderr, "Needed 4 arguments.\n");
        exit(1);

    }
    while (i < argc) {
        if(strcmp(argv[i], "-p") == 0)
            port = atoi(argv[i+1]);
        else if(strcmp(argv[i], "-d") == 0)
        {
            dirname = strdup(argv[i + 1]);
            if(chdir(dirname) != 0)
            {
                fprintf(stderr, "Error directory! Try again.\n");
                exit(1);
            }
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) == NULL)
                perror("getcwd() error");
            printf("CWD: %s\n", cwd);
        }
    i++;
    }

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);      /* The given port */
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    /* Listen for connections */
    if (listen(sock, 5) < 0) perror_exit("listen");
    pthread_t thr;
    printf("Listening for connections to port %d\n", port);
    while (1) { clientlen = sizeof(client);
        /* accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) perror_exit("accept");
        /* Find client's address */
    	if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL) {
          	    herror("gethostbyaddr"); exit(1);}
          	printf("Accepted connection from %s\n", rem->h_name);

        pthread_create(&thr, NULL, child_server, &newsock);
        break;
        close(newsock);
    }
    pthread_join(thr, NULL);

}


/* it would be very bad if someone passed us an dirname like
 * "; rm *"  and we naively created a command  "ls ; rm *".
 * So..we remove everything but slashes and alphanumerics.
 */
void sanitize(char *str)
{
    char *src, *dest;
    for ( src = dest = str ; *src ; src++ )
        if ( *src == '/' || isalnum(*src) )
            *dest++ = *src;
    *dest = '\0';
}


int cutLine(char *line, char **word)
{
    int i = 0;
    char *tmp = NULL;
    tmp = strdup(line);
    char *tmp1 = tmp;
    char delimit[]=" \n\t";

    char *token;
    int k = 0;
    k = (int) strlen(tmp);
    for (int j = 0; j < k; ++j) {
        printf("%c\n", tmp[j]);
    }

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
    for (int l = 0; l < i; ++l) {
        printf("%s\n", word[l]);
    }
    return i;
}
