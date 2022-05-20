#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
/*
    Implementação do problema clássico dos produtores/escritores com semáforos

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 19/05/2022

*/

//semaforo para controlar os leitores e o acesso ao arquivo
sem_t mutex, arquivo;
sem_t file;

//controla o numero de leitores do arquivo em simultaneo
int numLeitoresSimultaneos = 0;

//simula a leitura do arquivo
void lerArquivo(int id){
    printf("O leitor %d está lendo o arquivo\n", id);
    sleep(3);
}

//simula a escrita no arquivo
void escreveArquivo(){
    printf("O escritor está escrevendo no arquivo\n");
    sleep(3);
}

void *leitor(void *ptr){
    int id = (intptr_t)ptr;
    while(1) {
        //controla a fila dos leitores
        printf("O leitor %d está esperando para ler o arquivo\n", id);
        sem_wait(&mutex);
        numLeitoresSimultaneos++;
        //o primeiro leito bloqueia o arquivo, a leitura simultanea é simulada
        if(numLeitoresSimultaneos == 1){
            sem_wait(&file);
        }
        sem_post(&mutex);
        //le o arquivo
        lerArquivo(id);
        //espera pra sair da leitura
        sem_wait(&mutex);
        numLeitoresSimultaneos--;
        //libera a leitura do arquivo
        if(numLeitoresSimultaneos == 0){
            sem_post(&file);
        }
        printf("O leitor %d parou de ler o arquivo\n", id);
        sem_post(&mutex);
        sleep(1);
    }
}

void *escritor(void* data){
    while(1) {
        //espera para gravar no arquivo
        printf("O escritor está aguardando para escrever no arquivo\n");
        sem_wait(&file);
        //escreve no arquivo
        escreveArquivo();
        //libera o acesso ao arquivo
        sem_post(&file);
        printf("O escritor terminou de escrever no arquivo\n");
        sleep(1);
    }
}

int main(void){

    //inicializa os semaforos
    sem_init(&mutex, 0, 1);
    sem_init(&file, 0, 1);

    //pergunta o numero de leitores
    int numLeitores;
    printf("\nDigite a quantidade de Leitores: ");
    scanf("%d", &numLeitores);

    // cria as threads e faz a inicialização
    pthread_t ThreadLeitores[numLeitores];
    pthread_t ThreadEscritor;

    int i = 0;
    for (i = 0; i < numLeitores; i++){
        pthread_create(&ThreadLeitores[i], NULL, leitor, (void *)(intptr_t)i);
    }

    pthread_create(&ThreadEscritor, NULL, escritor, NULL);

    for (i = 0; i < numLeitores; i++){
        pthread_join(ThreadLeitores[i], NULL);
    }

    pthread_join(ThreadEscritor, NULL);

    //destroi os semaforos no final
    sem_destroy(&mutex);
    sem_destroy(&file);

    return 0;
}