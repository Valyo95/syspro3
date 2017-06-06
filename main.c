/* inet_str_client.c: Internet stream sockets client */
#include <stdio.h>
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <netdb.h>	         /* gethostbyaddr */
#include <stdlib.h>	         /* exit */
#include <string.h>	         /* strlen */

#define  BUFFSIZE 2000
#define MAXWORDS 300

#define  PORTNUM 2222

void perror_exit(char *message);

int write_all(int fd, void *buff, size_t size) {
    int sent, n;
    for(sent = 0; sent < size; sent+=n) {
        if ((n = write(fd, buff+sent, size-sent)) == -1)
            return -1; /* error */
    }
    return sent;
}

int main(int argc, char *argv[]) {
    char *buff = malloc(BUFFSIZE*sizeof(char));
    struct sockaddr_in  servadd; /* The address of server */
    struct hostent *hp;           /* to resolve server ip */
    int    sock, n_read;     /* socket and message length */
    char   buffer[BUFFSIZE];        /* to receive message */

    if ( argc != 3 ) {
        puts("Usage: rls <hostname> <directory>");exit(1);}
    /* Step 1: Get a socket */
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
        perror_exit( "socket" );
    /* Step 2: lookup server's address and connect there */
    if ((hp = gethostbyname(argv[1])) == NULL) {
        herror("gethostbyname"); exit(1);}
    memcpy(&servadd.sin_addr, hp->h_addr, hp->h_length);
    servadd.sin_port = htons(PORTNUM); /* set port number */
    servadd.sin_family = AF_INET ;     /* set  socket type */
    if (connect(sock, (struct sockaddr*) &servadd,
                sizeof(servadd)) !=0)
        perror_exit( "connect" );

    char *command = malloc(BUFFSIZE*sizeof(char));
    command[0] = '\0';
    strcat(command, "LIST");
    int j = 0;

    while ( command[j] != '\0')
    {
        if( write(sock,&command[j],1) != 1) {
            perror_exit("write");
        }
        printf("%c\n", command[j]);
        j++;
    }

    /* Step 4: read back results and send them to stdout */
    char buf[1];
    printf("writed\ngonna read now\n");
    int i = 0;


    read(sock, buf, 2);
    printf("I readed\n\n");


    printf("closing\n\n");
    close(sock);
    return 0;
}

void perror_exit(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}