#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>

// variaveis para o array, o numero de threads a serem criada e o numero escolhido pelo usuário
int *array, numThreads, numeroEscolhido;

// procura o numero escolhido em um trecho especifico do array, toda threads iria pesquisar o numero na mesma quantidade de posições
void* procuraNumeroVetor(void* thread){
    for(int i = (((int)(intptr_t)thread * numThreads) - numThreads); i < (((int)(intptr_t)thread * numThreads) - 1); i++){
        if(array[i] == numeroEscolhido){
            printf("Thread %d encontrou o numero %d\n", (int)(intptr_t)thread, numeroEscolhido);
            return 0;
        }
    }
    printf("A Thread %d pesquisou no array entre as posições %d e %d, porém, não encontrou o numero %d\n", (int)(intptr_t)thread, (((int)(intptr_t)thread * numThreads) - numThreads), (((int)(intptr_t)thread * numThreads) - 1), numeroEscolhido);
    return 0;   
}


int main() {
    // pega o numero de threads que o usuário deseja
    printf("Digite o numero de Threads a serem utilizadas: ");
    scanf("%d", &numThreads);

    // pega o numero que o usuário escolheu
    printf("Digite um numero entre 0 e %d: ", ((numThreads * numThreads) - 1));
    scanf("%d", &numeroEscolhido);

    // aloca o tamanho correto para o array
    array = malloc(((numThreads * numThreads) - 1) * sizeof(int));

    // preenche o array com numeros inteiros sequenciais
    for(int i = 0; i < ((numThreads * numThreads) - 1); i++){
        array[i] = i;
    }

    printf("\nArray criado: [");
    for(int i = 0; i < ((numThreads * numThreads) - 1); i++){
        printf("%d, ", array[i]);
    }
    printf("]\n\n");

    // array de threads
    pthread_t arrayThread[numThreads];

    // criação das threads
	for (int i = 0; i < numThreads; i++){
	    pthread_create(&arrayThread[i], NULL, procuraNumeroVetor, (void *)(intptr_t)i+1);
	}
	
     // Espera todas as threads terminarem
     
    for(int i = 0; i < numThreads; i++){
        pthread_join(arrayThread[i], NULL);
    }

    return 0;
}