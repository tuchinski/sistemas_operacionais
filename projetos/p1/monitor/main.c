#include "monitor.h"

void *ouvinte(void* ptr){
    int id_ouvinte = * (int*) ptr;

    comp_entrar_sala(id_ouvinte);
}

int main(int argc, char const *argv[])
{
    init_monitor();
    int teste = 10;
    pthread_t threads_ouvinte[teste];
       for (int i = 0; i < teste; i++) // inicia as threads dos clientes (chegada dos clientes na barbearia)
    {
        pthread_create(&threads_ouvinte[i], NULL, ouvinte, (void*)&i);
        sleep(1); // frequencia com que os clientes chegam
    }

    for (int i = 0; i < teste; i++) // espera todas as threads de clientes terminarem
    {
        pthread_join(threads_ouvinte[i], NULL);
    
    }
    destroy_monitor();
    return 0;
}
