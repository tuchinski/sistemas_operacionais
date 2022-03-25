#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    Faça um programa que receba um comando Linux como parâmetro e execute como um filho do
    processo. O processo pai deve aguardar o término da execução do comando.

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 24/03/2022

*/

int main(int argc, char *argv[]){
    pid_t child_pid;
    int status;

    // concatenar o comando com o path
    char comando[] = "/bin/";
    strcat(comando, argv[1]);
    
    // cria o processo filho
    child_pid = fork();

    // caso sucesso no fork
    if (child_pid >= 0){
        if (child_pid == 0){
            //filho executa o comando
            printf("Filho iniciando execução do comando: %s\n", comando);
            execl(comando, argv[1], (char *) NULL);
            printf("filho encerra\n");
        }
        else{
            //pai espera a execução do filho
             printf("Pai espera o filho executar o comando...\n");
             wait(&status); 
             printf("Pai encerra\n");
             exit(0);
        }
    }
    // caso erro no fork
    else{
        exit(0);
    }
} 