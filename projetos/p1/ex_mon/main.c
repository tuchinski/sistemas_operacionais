#include "monitor.h"


// thread do aluno de computacao
void *ouvinte(void* ptr){
    int id_ouvinte = * (int*) ptr;

    comp_entrar_sala(id_ouvinte);
    sleep(1);
}

//thread do aluno de so
void *aluno_so(void* ptr){
    int id_aluno_so = * (int*) ptr;
    so_entrar_sala(id_aluno_so);
}

// thread do professor
void *professor(void* ptr){
    int num_alunos_so = (int*) ptr;
    
    // realiza (num_alunos_so/2) execucoes do professor
    for (int i = 0; i < num_alunos_so/2; i++){
        while(p_liberar_entrada() == -1){
            sleep(1);
        }
            
        p_iniciar_apresentacoes();
    }
    
    while(p_fechar_porta() == -1){
        sleep(1);
    }
}


/* 
 * metodo main
 */
int main(int argc, char const *argv[])
{
    init_monitor();

    int num_alunos_so = 4; //
    int num_alunos_comp = 5*(num_alunos_so/2);
    pthread_t threads_ouvinte[num_alunos_comp];
    pthread_t threads_alunos_so[num_alunos_so];
    pthread_t prof;
    
    pthread_create(&prof, NULL, professor, (void*)num_alunos_so);
    sleep(1);

    for (int i = 0; i < num_alunos_comp; i++) {
        pthread_create(&threads_ouvinte[i], NULL, ouvinte, (void*)&i);
        sleep(1); 
    }

    for (int i = 0; i < num_alunos_so; i++)
    {
        pthread_create(&threads_alunos_so[i], NULL, aluno_so, (void*)&i);
    }
    

    for (int i = 0; i < num_alunos_comp; i++) // espera todas as threads de clientes terminarem
    {
        pthread_join(threads_ouvinte[i], NULL);
    
    }


    destroy_monitor();
    printf("--------------- FINALIZANDO A EXECUCAO ---------------");
    return 0;
}
