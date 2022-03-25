#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void cria_filho(int qtd_filho){
    if (qtd_filho == 0){
        return;
        //exit (0);
    }

    pid_t filho_esq;
    pid_t filho_dir;

    filho_esq = fork();

    if (filho_esq){
        filho_dir = fork();
        if (filho_dir){
            sleep(10);
            return;
        }
               
    }
    cria_filho(qtd_filho-1);
}

int main(int argc, char const *argv[])
{
    int nivel = 3;
    cria_filho(nivel);
}

