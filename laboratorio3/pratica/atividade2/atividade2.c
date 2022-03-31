#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define TAM 20

/*
    Faça um programa com 2 threads que calcule a frequência de elementos em um vetor que contem
    somente valores de 0 a 9

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 31/03/2022

*/

struct data_chunk
{
    int* vetor;
    int tam_inicial;
    int tam_final;
    int* res;
};

//contar a frequencia de 0-9
void* frequencia(void* param){
    struct data_chunk* dados = param;
    for (int i = dados->tam_inicial; i < dados->tam_final; i++){
        dados->res[dados->vetor[i]]++;
    }
    return NULL;
}

int main(int argc, char const *argv[]){
    int vetor[TAM];
    int* res = (int*) calloc(10, sizeof(int));

    //preenche o vetor aleatoriamente
    srand(time(NULL));
    for (int i = 0; i < TAM; i++)
    {
        vetor[i] = rand() % 10;
    }
    
    pthread_t th1,th2;

    struct data_chunk dados1, dados2;

    //populando dados para thread
    dados1.vetor = vetor;
    dados1.tam_inicial = 0;
    dados1.tam_final = TAM/2;
    dados1.res = res;
    
    dados2.vetor = vetor;
    dados2.tam_inicial = TAM/2;
    dados2.tam_final = TAM;
    dados2.res = res;

    pthread_create(&th1, NULL, frequencia, &dados1);
    pthread_create(&th2, NULL, frequencia, &dados2);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    for (int i = 0; i < TAM; i++){
            printf(" %i ", vetor[i]);
    }
    printf("\n");

    for (int i = 0; i < 10; i++){
            printf(" %i -> %i ", i, res[i]);
    }


    return 0;
}
