#include "barbeiro_monitor.h"

/*
    Implementação do problema clássico do barbeiro dorminhoco com monitores

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 19/05/2022

*/

/*
 * Função que a thread que representa o barbeiro irá executar
 */
void *barbeiro(void* ptr){
    while (1)
    {
        printf("Barbeiro livre para cortar um cabelo\n");
        atenderCliente();
        printf("Barbeiro terminou de cortar e vai se preparar para atender o proximo cliente\n\n\n");
        sleep(TEMPO_ATENDIMENTO_BARBEIRO); //tempo para o barbeiro se preparar para o proximo corte

    }
    
}

/*
 * Função que a thread que representa o cliente irá executar
 */
void *cliente(void* ptr){
    int id_cliente = * (int*) ptr;
    printf("\n\nCliente %i chegando na barbearia\n", id_cliente);
    if(chegarBarbearia(id_cliente)){
        printf("Cliente %i atendido com sucesso\n", id_cliente);
    }else{
        printf("Barbearia cheia, cliente %i indo embora\n\n\n", id_cliente);
    }
    pthread_exit(0);
}

int main(int argc, char const *argv[])
{
    pthread_t arr[QTDE_CLIENTES_ATENDIMENTO]; // array que armazena as threads dos clientes
    pthread_t barb; // thread que representa o barbeiro

    initMonitor();
    pthread_create(&barb, NULL, barbeiro, NULL); // inicia a thread do barbeiro
    
    for (int i = 0; i < QTDE_CLIENTES_ATENDIMENTO; i++) // inicia as threads dos clientes (chegada dos clientes na barbearia)
    {
        pthread_create(&arr[i], NULL, cliente, (void*)&i);
        sleep(FREQUENCIA_CHEGADA_CLIENTES); // frequencia com que os clientes chegam
    }

    for (int i = 0; i < QTDE_CLIENTES_ATENDIMENTO; i++) // espera todas as threads de clientes terminarem
    {
        pthread_join(arr[i], NULL);
    }


    destroyMonitor(); //finaliza os monitores
    
}
