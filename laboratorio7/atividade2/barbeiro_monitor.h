#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_CLIENTES 5
#define TEMPO_ATENDIMENTO_BARBEIRO 1
#define QTDE_CLIENTES_ATENDIMENTO 10
#define FREQUENCIA_CHEGADA_CLIENTES 1

// controle de concorrencia
pthread_mutex_t the_mutex;
pthread_cond_t condBarbeiro, condClientes;

int num_clientes;

/* --- monitor operations --- */
void initMonitor();
void destroyMonitor();
void atenderCliente();
int chegarBarbearia();
