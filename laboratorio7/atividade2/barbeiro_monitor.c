#include "barbeiro_monitor.h"

/*
    Inicia o monitor, criando o mutex e as variaveis de condicao
*/
void initMonitor(){
    pthread_mutex_init(&the_mutex, NULL);
    pthread_cond_init(&condBarbeiro, NULL);
    pthread_cond_init(&condClientes, NULL);
    num_clientes = 0;
}

/*
 * Finaliza o monitor, destruindo o mutex e as variaveis de condicao 
 */
void destroyMonitor(){
     pthread_mutex_destroy(&the_mutex); 
    pthread_cond_destroy(&condBarbeiro);      
    pthread_cond_destroy(&condClientes);
}

void atenderCliente();
int chegarBarbearia();
