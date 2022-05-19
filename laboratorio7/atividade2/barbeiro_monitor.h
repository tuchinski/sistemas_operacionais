#include <pthread.h>
#define MAX_CLIENTES 5

// controle de concorrencia
pthread_mutex_t the_mutex;
pthread_cond_t condBarbeiro, condClientes;

/* --- resource --- */
int num_clientes;   // 1 true, 0 false

/* --- monitor operations --- */
void initMonitor();
void destroyMonitor();
void atenderCliente();
int chegarBarbearia();
