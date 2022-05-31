#include "monitor.h"

void *ouvinte(void* ptr){
    int id_ouvinte = * (int*) ptr;

    comp_entrar_sala(id_ouvinte);
    sleep(1);
}

void *professor(void* ptr){
    while(p_liberar_entrada() == -1){
        sleep(1);
    }
        
    p_iniciar_apresentacoes();
    // sleep();
    // int a = 
    // printf("!!!!!!!! %i !!!!!!", a);
    while(p_liberar_entrada() == -1){
        printf("teste");
        sleep(1);
    }
    p_iniciar_apresentacoes();
}

int main(int argc, char const *argv[])
{
    init_monitor();

    int teste = 10;
    pthread_t threads_ouvinte[teste];
    pthread_t prof;
    
    pthread_create(&prof, NULL, professor, NULL);
    sleep(1);

    for (int i = 0; i < teste; i++) {
        pthread_create(&threads_ouvinte[i], NULL, ouvinte, (void*)&i);
        sleep(1); 
    }

    for (int i = 0; i < teste; i++) // espera todas as threads de clientes terminarem
    {
        pthread_join(threads_ouvinte[i], NULL);
    
    }


    sleep(10);
    destroy_monitor();
    return 0;
}
