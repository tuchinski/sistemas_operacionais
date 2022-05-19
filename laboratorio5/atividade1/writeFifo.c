#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>

#define SERVER_FIFO "/tmp/serverfifo"

char buf [512];


int main (int argc, char **argv){
    int fd_server;

    if ((fd_server = open (SERVER_FIFO, O_RDWR)) == -1) {
        perror ("open: server fifo");
        return 1;
    }

    char* buf = (char*)calloc(1, sizeof(char));

    printf("Digite a operação a ser realizada: ");
    scanf("%[^\n]", buf);

    write (fd_server, buf, strlen(buf));

    if (close (fd_server) == -1) {
       perror ("close");
       return 2;
    }
         
    return 0;
}