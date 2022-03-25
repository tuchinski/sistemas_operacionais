#include <stdio.h>
#include <time.h> // time
#include <stdlib.h> // rand
#include <sys/types.h> // pid_t
#include <unistd.h>    //fork()
#include <sys/wait.h>
#define TAMANHO_DIVISAO 2


/*
    Faça um programa que receba um vetor e divida para N filhos partes iguais de processamento para
    localizar um item. Exibir o PID dos filhos que encontrarem o valor procurado

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 24/03/2022

*/

void busca_vetor_filhos(int *vetor, int inicio_busca, int fim_busca, int num_buscado)
{
    pid_t pid;
    pid = fork();

    if (!pid)
    {
        for (int i = inicio_busca; i <= fim_busca; i++)
        {
            if (vetor[i] == num_buscado)
            {
                printf("NUMERO ENCONTRADO - Indice -> %i\n", i);
                exit(0);
            }
        }
        exit(0);
    }
}

int main(int argc, char const *argv[])
{
    // Declarando o vetor e seu tamanho
    int tam_vetor = 10;
    int vetor[tam_vetor];
    int qtde_filhos;
    int qtde_itens_processados;
    int inicio;
    int fim;
    int numero_buscado;

    srand(time(NULL));

    // preenchendo o vetor com tamanho aleatorio
    for (int i = 0; i < tam_vetor; i++)
    {
        vetor[i] = rand() % 25;
    }

    printf("Imprimindo o vetor gerado -> ");
    for (int i = 0; i < tam_vetor; i++)
    {
        printf("%d - ", vetor[i]);
    }
    printf("\nDigite o número a ser buscado: ");

    scanf("%i", &numero_buscado);

    // Realizando a divisao dos vetores
    qtde_filhos = TAMANHO_DIVISAO;
    if (qtde_filhos <= tam_vetor)
    {
        qtde_itens_processados = tam_vetor / qtde_filhos;

        inicio = 0;
        fim = qtde_itens_processados - 1;

        for (int i = 0; i < qtde_filhos; i++)
        {
            if (qtde_filhos - 1 == i)
            {
                fim = fim + (tam_vetor % qtde_filhos);
            }
            busca_vetor_filhos(vetor, inicio, fim, numero_buscado);

            inicio = fim + 1;
            fim = fim + qtde_itens_processados;
        }
    }
    else
    {
        printf("Numero de processos filhos deve ser menor ou igual ao tamanho do vetor -> %d", tam_vetor);
    }

    // sleep(2);
    return 0;
}
