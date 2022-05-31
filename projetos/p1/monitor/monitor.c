#include "monitor.h"


// -------------------- Metodos do monitor --------------------
// Metodo que inicia o monitor
void init_monitor(){

    num_max_alunos_comp_sala = 5;
    num_min_alunos_comp_sala = 1;
    num_min_alunos_so_sala = 2;
    qtde_atual_alunos_comp_sala = 0;
    qtde_atual_alunos_so_sala = 0;
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
int p_liberar_entrada(){
    pthread_mutex_lock(&regiao_critica);
    // precisa esperar todos os alunos de computacao
    // aceitaOuvintes = 1; // permitindo que a sala aceite ouvintes
    if(qtde_atual_alunos_comp_sala != 0){
        printf("PROFESSOR precisa aguardar a saida de todos os alunos\n"); 
        pthread_mutex_unlock(&regiao_critica);
        return -1;
    }
    apresentacao = 0; // informa que nao esta acontecendo uma apresentacao
    printf("A entrada foi liberada pelo PROFESSOR\n");

    // acorda todos os alunos
    pthread_cond_broadcast(&fila_alunos_comp);
    pthread_cond_broadcast(&fila_alunos_so);

    pthread_mutex_unlock(&regiao_critica);
    return 1;
} 

/*
 *
 *
 * tem que acordar 2 alunos de so e 5 alunos de computação
 * 
 */ 
void p_iniciar_apresentacoes(){
    pthread_mutex_lock(&regiao_critica);

    printf("\n\nPROFESSOR quer iniciar as apresentacoes\n\n");
    
    // espera que os alunos de computacao entrem na sala
    while(qtde_atual_alunos_comp_sala != num_max_alunos_comp_sala){
        printf("PROFESSOR esperando os alunos. Total de alunos na sala = %i\n\n", qtde_atual_alunos_comp_sala);
        pthread_cond_wait(&cond_professor_so, &regiao_critica);
    }
    printf("PROFESSOR avisa que a sala esta lotada para os alunos de Comp.\n\n");

    // ! descomentar esse trecho quando estiver implementada a entrada dos alunos de SO
    // espera que todos os alunos de SO entrem na sala
    // while(qtde_atual_alunos_so_sala != 2){
    //     printf("PROFESSOR esperando os alunos. Total de alunos na sala = %i\n\n", qtde_atual_alunos_comp_sala);
    //     pthread_cond_wait(&cond_professor_so, &regiao_critica);
    // }
    // printf("PROFESSOR avisa que a sala esta lotada para os alunos de SO\n\n");

    printf("\nPROFESSOR inicia as apresentacoes\n");
    
    // setando a var apresentacao para 1, definindo que existe uma apresentacao acontecendo na sala
    // e acordando todos os processos que estavam esperando o inicio da apresentacao
    apresentacao = 1;
    pthread_cond_broadcast(&cond_apresentacao);

    pthread_mutex_unlock(&regiao_critica);
} 

// Atribui nota para um aluno especifico
void p_atribuir_nota(int id_aluno, int nota){
    printf("\n\no PROFESSOR atribuiu a nota %i para o aluno %i\n\n", nota, id_aluno);
}

// quando nao tem mais ninguem pra apresentar, termina a execução
int p_fechar_porta(){
    pthread_mutex_lock(&regiao_critica);

    if(qtde_atual_alunos_comp_sala != 0 || qtde_atual_alunos_so_sala != 0){
        printf("PROFESSOR precisa aguardar a saida de todos os alunos para fechar a sala\n"); 
        pthread_mutex_unlock(&regiao_critica);
        return -1;
    }

    printf("\n\nO PROFESSOR esta fechando a sala de apresentacoes\n\n");
    apresentacao = 1; // coloca o valor de apresentacao em 1, nao permitindo mais a entrada de nenhum aluno

    pthread_mutex_unlock(&regiao_critica);
    return 1;
}

// -------------------- Fim Metodos do professor ---------------------


// -------------------- Metodos dos alunos de SO ---------------------

/*
 * Metodo em que o aluno de SO tenta entrar na sala para apresentar
 *
 */ 
void so_entrar_sala(int id_aluno){

    printf("O aluno %i de SO, entrou na sala", id_aluno);

}
 
void so_assinar_lista_entrada(int id_aluno){ // dentro do so_entrar_sala

    printf("O aluno %i de SO assinou a lista de entrada\n", id_aluno);

}

void so_aguardar_apresentacoes(int id_aluno){ // quando o aluno não conseguir apresentar, fica esperando

    printf("O aluno %i de SO aguarda apresentação", id_aluno);

} 

void so_apresentar(int id_aluno){ // aluno de SO apresenta

    printf("O aluno %i inicia a apresentação", id_aluno);

}

void so_assinar_lista_saida(int id_aluno){ // tem que assinar quando for sair da sala

    printf("O aluno %i de SO assinou a lista de saída\n", id_aluno);

} 
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
    while(apresentacao == 1  || qtde_atual_alunos_comp_sala >= num_max_alunos_comp_sala){
        printf("Aluno de Comp. %i aguardando a sala liberar para ouvintes\n\n", id_aluno);
        pthread_cond_wait(&fila_alunos_comp, &regiao_critica); // espera ate que tenha vagas na sala
    }

    qtde_atual_alunos_comp_sala++;
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
    printf("\nO aluno de Comp. %i esta assistindo a apresentacao\n\n", id_aluno);
} 

/*
 * Metodo que define o comportamento do aluno de computacao ao sair da apresentação
 * Parametros:
 * int id_aluno -> id do aluno para melhor visualizacao
 */
void comp_sair_apresentacao(int id_aluno){
    printf("\nO aluno de Comp. %i saiu da apresentacao\n\n", id_aluno);
    qtde_atual_alunos_comp_sala--;
    sleep(1);
}

// -------------------- Fim Metodos dos alunos de computação ---------------------
