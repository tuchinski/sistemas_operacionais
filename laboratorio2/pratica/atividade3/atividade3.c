#include <stdio.h>
#include <time.h> // time
#include <stdlib.h> // rand
#include <sys/types.h> // pid_t
#include <unistd.h>    //fork()
#define print printf

int *busca_vetor_filhos(int *vetor, int inicio_busca, int fim_busca, int num_buscado)
{
    pid_t pid;
    pid = fork();

    if (pid)
    {
        printf("\nFilho criado: %d\n", pid);
        for (int i = inicio_busca; i <= fim_busca; i++)
        {
            if (vetor[i] == num_buscado)
            {
                printf("NUMERO ENCONTRADO NO FILHO COM PID: %d\n", pid);
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
    qtde_filhos = 5;
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
            // printf("[%d] -> Inicio: %d, Fim: %d\n", i, inicio, fim);

            inicio = fim + 1;
            fim = fim + qtde_itens_processados;
        }
    }
    else
    {
        printf("Numero de processos filhos deve ser menor ou igual ao tamanho do vetor -> %d", tam_vetor);
    }

    return 0;
}
