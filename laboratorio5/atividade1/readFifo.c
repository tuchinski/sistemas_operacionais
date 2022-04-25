#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_FIFO "/tmp/serverfifo"

char buf[512];

// função que irá realizar os calculos
float calc(char str[]){
    int num1 = str[0] - '0';
    int num2 = str[4] - '0';

    if(str[2] == '+'){
        return num1 + num2;
    }
    else if(str[2] == '-'){
        return num1 - num2;
    }
    else if(str[2] == '*'){
        return num1 * num2;
    }
    else{
        return num1 / num2;
    }
}

int main(int argc, char **argv){
    int fd, bytes_read;

    // cria um FIFO caso não exista
    if ((mkfifo(SERVER_FIFO, 0664) == -1) && (errno != EEXIST)){
        perror("mkfifo");
        exit(1);
    }

    // inicializa o FIFO
    if ((fd = open(SERVER_FIFO, O_RDONLY)) == -1){
        perror("open");
    }

    memset(buf, '\0', sizeof(buf));

    // verificação da leitura
    if ((bytes_read = read(fd, buf, sizeof(buf))) == -1){
        perror("read");
    }

    if (bytes_read == 0){
        printf("Erro na leitura do FIFO\nIniciando outro FIFO...\n");
        close(fd);
        fd = open(SERVER_FIFO, O_RDONLY);
        return 0;
    }

    // chama a função para realizar o calculo
    if (bytes_read > 0){
        printf("Operação recebida: %s\nResultado: %.2f\n", buf, calc(buf));
    }

    if (close(fd) == -1){
        perror("close");
    }

    return 0;
}
