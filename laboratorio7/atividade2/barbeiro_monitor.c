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

/*
 * Metodo que o barbeiro executa para atender um cliente
 */
void atenderCliente(){
    // printf("Entrando atenderCliente\n");
    pthread_mutex_lock(&the_mutex); //pega o mutex
    while (num_clientes == 0){
        printf("Barbeiro sem clientes para atender. Dormindo! zzzZZzzZzZZzZZZz\n");
        pthread_cond_wait(&condBarbeiro, &the_mutex); // barbeiro espera por clientes e libera o mutex
        printf("Chegou um cliente para o barbeiro!!!\n");
    }
    num_clientes--;
    pthread_cond_signal(&condClientes); // acorda os clientes pra poder atender

    pthread_mutex_unlock(&the_mutex); // libera o mutex
}

/*
 * Metodo que o cliente executa ao chegar na barbearia
 * Caso a barbearia esteja cheia (num_clientes == MAX_CLIENTES) o cliente vai embora e o metodo retorna o valor 0
 * Caso o cliente seja atendido, o metodo retorna o valor 1
 */
int chegarBarbearia(int id_cliente){
    // printf("Entrando chegarBarbearia\n");
    pthread_mutex_lock(&the_mutex); //pega o mutex

    if(num_clientes == MAX_CLIENTES){
        // Limite maximo de clientes em espera, se entrou aqui ele tem que ir embora
        pthread_mutex_unlock(&the_mutex);
        return 0;
    }else{
        num_clientes++;
        pthread_cond_signal(&condBarbeiro);
        printf("Cliente %i esta esperando o barbeiro\n",id_cliente); 
        pthread_cond_wait(&condClientes, &the_mutex);

        printf("Cliente %i esta cortando o cabelo!!\n", id_cliente);
    }

    pthread_mutex_unlock(&the_mutex); // libera o mutex
    // printf("Saindo chegarBarbearia\n");
    return 1;
}
