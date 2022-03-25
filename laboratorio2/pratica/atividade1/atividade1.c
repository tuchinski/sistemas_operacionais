#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#define NIVEL_ARVORE 2
#define TEMPO_ESPERA 10

/*
    Faça um programa que crie uma hierarquia de processos com N níveis (1 + 2 + 4 + 8 + ... + 2 N-1 )
    processos. Visualize a hierarquia usando um comando do sistema (pstree).

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 24/03/2022

*/

// Cria os n filhos
void cria_filho(int qtd_filho){
    // condicao de parada da criacao dos filhos
    if (qtd_filho == 0){
        return;
        //exit (0);
    }

    pid_t filho_esq;
    pid_t filho_dir;

    // pai cria o filho esquerdo
    filho_esq = fork();

    if (filho_esq){
        // pai cria o filho direito
        filho_dir = fork();

        // depois da criacao dos fihos, o pai pode sair da funcao
        if (filho_dir){
            sleep(TEMPO_ESPERA);
            return;
        }
               
    }
    // os filhos criam seus filhos, se necessario
    cria_filho(qtd_filho-1);
}

int main(int argc, char const *argv[])
{
    int pid = getppid();
    printf("Para verificar a árvore de processos, use o comando \"pstree -a %i\" \n", pid);
    int nivel = NIVEL_ARVORE;

    cria_filho(nivel);
}

