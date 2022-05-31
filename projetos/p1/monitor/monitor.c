#include "monitor.h"


// -------------------- Metodos do monitor --------------------
// Metodo que inicia o monitor
void init_monitor(){

    num_max_alunos_comp_sala = 5;
    num_min_alunos_comp_sala = 1;
    num_min_alunos_so_sala = 2;
    qtde_atual_alunos_sala = 0;
    apresentacao = 0; // boolean para definir se esta acontecendo alguma apresentação
    aceitaOuvintes = 1; //para definir se a apresentação ainda aceita ouvintes
    pthread_mutex_init(&regiao_critica, NULL);
    pthread_mutex_init(&sala, NULL);
    pthread_cond_init(&fila_alunos_comp, NULL);
    pthread_cond_init(&fila_alunos_so, NULL);
    pthread_cond_init(&cond_professor_so, NULL);
    pthread_cond_init(&cond_apresentacao, NULL);
}

/* 
 * Metodo que destroi o monitor 
 */
void destroy_monitor(){
    pthread_mutex_destroy(&regiao_critica);
    pthread_mutex_destroy(&sala);
    pthread_cond_destroy(&fila_alunos_comp);
    pthread_cond_destroy(&fila_alunos_so);
    pthread_cond_destroy(&cond_professor_so);
    pthread_cond_destroy(&cond_apresentacao);
}
// -------------------- Fim Metodos do monitor --------------------

// -------------------- Metodos do professor ---------------------
/* dispara o for que vai acordar os alunos de computacao e de SO

*/
void p_liberar_entrada(){
    pthread_mutex_lock(&regiao_critica);
    // precisa esperar todos os alunos de computacao
    aceitaOuvintes = 1; // permitindo que a sala aceite ouvintes
    while(qtde_atual_alunos_sala != 0){
        printf("Professor aguardando a saida de todos os alunos\n");
        sleep(0.5); // ! cuidar com o sleep aqui
    }
    printf("A entrada foi liberada pelo professor\n");

    // acorda todos os alunos
    pthread_cond_broadcast(&fila_alunos_comp);
    pthread_cond_broadcast(&fila_alunos_so);

    while(qtde_atual_alunos_sala != 5){
        printf("Professor esperando os alunos. Total de alunos na sala = %i\n\n", qtde_atual_alunos_sala);
        pthread_cond_wait(&cond_professor_so, &regiao_critica);
    }
    printf("Professor avisa que a sala esta lotada para os alunos de Comp.\n\n");

    // while(qtde_atual_alunos_sala != 5){
    //     pthread_cond_wait(&cond_professor_so, &regiao_critica);
    // }

    // precisa esperar todos os alunos de SO

    pthread_mutex_unlock(&regiao_critica);
} 
void p_iniciar_apresentacoes(); // tem que acordar 2 alunos de so e 5 alunos de computação
void p_atribuir_nota();
void p_fechar_porta(); // quando nao tem mais ninguem pra apresentar, termina a execução
// -------------------- Fim Metodos do professor ---------------------


// -------------------- Metodos dos alunos de SO ---------------------

/*
 * Metodo em que o aluno de SO tenta entrar na sala para apresentar
 *
 */ 
void so_entrar_sala(int id_aluno);
void so_assinar_lista_entrada(int id_aluno); // dentro do so_entrar_sala
void so_aguardar_apresentacoes(int id_aluno); // quando o aluno não conseguir apresentar, fica esperando
void so_apresentar(int id_aluno); // aluno de SO apresenta
void so_assinar_lista_saida(int id_aluno); // tem que assinar quando for sair da sala
// -------------------- Fim Metodos dos alunos de SO ---------------------


// -------------------- Metodos dos alunos de computação ---------------------

/*
 * Metodo em que o aluno de computacao tenta entrar em uma sala para assistir
 * a apresentação
 * 
 * Parametros:
 * int id_aluno -> id do aluno para melhor visualizacao
 */
void comp_entrar_sala(int id_aluno){
    pthread_mutex_lock(&regiao_critica); // protegendo o metodo

    printf("Aluno de Comp. %i tentando entrar na sala\n", id_aluno);

    // * Caso a sala não aceite mais ouvintes(já iniciou a apresentação) 
    // * ou já está cheia, deixa o ouvinte esperando
    while(aceitaOuvintes == 1  && qtde_atual_alunos_sala >= num_max_alunos_comp_sala){
        printf("Aluno de Comp. %i aguardando a sala liberar para ouvintes\n\n", id_aluno);
        pthread_cond_wait(&fila_alunos_comp, &regiao_critica); // espera ate que tenha vagas na sala
    }

    qtde_atual_alunos_sala++;
    pthread_cond_signal(&cond_professor_so); // * Avisa o professor que chegou para assistir a apresentação
    printf("Aluno de Comp. %i entrou na sala e esta esperando o inicio da apresentacao\n\n", id_aluno);

    // * enquanto não iniciar a apresentação, o ouvinte espera
    while (apresentacao == 0){
        pthread_cond_wait(&cond_apresentacao, &regiao_critica);
    }
    comp_assistir_apresentacao(id_aluno);

    // TODO: meio de o aluno poder sair durante a apresentação 
    comp_sair_apresentacao(id_aluno);    

    pthread_mutex_unlock(&regiao_critica); // liberando o mutex
}

/*
 * Metodo que define o comportamento do aluno de computacao enquanto assiste uma apresentação 
 * Parametros:
 * int id_aluno -> id do aluno para melhor visualizacao
 */
void comp_assistir_apresentacao(int id_aluno){
    printf("O aluno de Comp. %i esta assistindo a apresentacao", id_aluno);
} 

/*
 * Metodo que define o comportamento do aluno de computacao ao sair da apresentação
 * Parametros:
 * int id_aluno -> id do aluno para melhor visualizacao
 */
void comp_sair_apresentacao(int id_aluno){
    printf("O aluno de Comp. %i saiu da apresentacao", id_aluno);
}

// -------------------- Fim Metodos dos alunos de computação ---------------------