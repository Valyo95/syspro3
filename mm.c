
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <bits/signum.h>

#define BUFFSIZE 2000
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
    char *buff = malloc(BUFFSIZE*sizeof(char));
    char *dir = "cmake-build-debug/CMakeFiles/clion-log.txt";
    FILE *pop = popen("find $PWD", "r");
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        perror("getcwd() error");

    while(fgets(buff, BUFFSIZE, pop)!=NULL){
        isDirMine(buff, dir);
    }


    return 0;
}

