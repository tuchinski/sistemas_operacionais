#include "monitor.h"


// -------------------- Metodos do monitor --------------------
// Metodo que inicia o monitor
void init_monitor(){

    num_max_alunos_comp_sala = 5;
    num_max_alunos_so_sala = 2;
    qtde_atual_alunos_comp_sala = 0;
    qtde_atual_alunos_so_sala = 0;
    apresentacao = 0; // boolean para definir se esta acontecendo alguma apresentação
    ordem_apresentacao[0] = -1;
    ordem_apresentacao[1] = -1;
    num_apresentacao = 0;
    total_apresentacao = 0;
    fim_apresentacoes = 0;
    pthread_mutex_init(&regiao_critica, NULL);
    pthread_cond_init(&fila_alunos_comp, NULL);
    pthread_cond_init(&fila_alunos_so, NULL);
    pthread_cond_init(&cond_professor_so, NULL);
    pthread_cond_init(&cond_apresentacao, NULL);
    pthread_cond_init(&cond_fim_apresentacao, NULL);
    srand(time(NULL)); // deixa a execucao do rand, realmente aleatoria
}

/* 
 * Metodo que destroi o monitor 
 */
void destroy_monitor(){
    pthread_mutex_destroy(&regiao_critica);
    pthread_cond_destroy(&fila_alunos_comp);
    pthread_cond_destroy(&fila_alunos_so);
    pthread_cond_destroy(&cond_professor_so);
    pthread_cond_destroy(&cond_apresentacao);
    pthread_cond_destroy(&cond_fim_apresentacao);
}
// -------------------- Fim Metodos do monitor --------------------

// -------------------- Metodos do professor ---------------------

int p_liberar_entrada(){
    pthread_mutex_lock(&regiao_critica);
    // precisa esperar todos os alunos de computacao
    if(qtde_atual_alunos_comp_sala != 0 || qtde_atual_alunos_so_sala != 0){
        printf("[PROFESSOR] -> precisa aguardar a saida de todos os alunos\n"); 
        printf("[DEBUG] qtde_atual_alunos_comp_sala -> %i ", qtde_atual_alunos_comp_sala);
        printf("[DEBUG] qtde_atual_alunos_so_sala -> %i ", qtde_atual_alunos_so_sala);
        pthread_mutex_unlock(&regiao_critica);
        return -1;
    }
    apresentacao = 0; // informa que nao esta acontecendo uma apresentacao
    printf("[PROFESSOR] -> A entrada foi liberada pelo PROFESSOR\n");
    ordem_apresentacao[0] = -1;
    ordem_apresentacao[1] = -1;
    num_apresentacao = 0;
    fim_apresentacoes = 0;
    total_apresentacao = 0;

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

    printf("\n\n[PROFESSOR] -> quer iniciar as apresentacoes\n\n");
    
    // espera que os alunos de computacao entrem na sala
    while(qtde_atual_alunos_comp_sala != num_max_alunos_comp_sala){
        printf("[PROFESSOR] -> esperando os alunos de Comp. Total de alunos na sala = %i\n\n", qtde_atual_alunos_comp_sala);
        pthread_cond_wait(&cond_professor_so, &regiao_critica);
    }
    printf("[PROFESSOR] -> avisa que a sala esta lotada para os alunos de Comp.\n\n");

    printf("[PROFESSOR] -> espera a chegada de alunos de SO\n\n");

    // espera que todos os alunos de SO entrem na sala
    while(qtde_atual_alunos_so_sala != 2){
        printf("[PROFESSOR] -> esperando os alunos de SO. Total de alunos na sala = %i\n\n", qtde_atual_alunos_so_sala);
        pthread_cond_wait(&cond_professor_so, &regiao_critica);
    }
    printf("[PROFESSOR] -> avisa que a sala esta lotada para os alunos de SO\n\n");

    printf("\n[PROFESSOR] -> inicia as apresentacoes\n");
    
    // setando a var apresentacao para 1, definindo que existe uma apresentacao acontecendo na sala
    // e acordando todos os processos que estavam esperando o inicio da apresentacao
    apresentacao = 1;
    pthread_cond_broadcast(&cond_apresentacao);

    // professor espera o fim de todas as apresentacoes para dar a nota
    while(total_apresentacao < 2){
        printf("\n[PROFESSOR] ->  esperando o termino das apresentacoes\n\n");
        pthread_cond_wait(&cond_apresentacao, &regiao_critica);
    }

    for (int i = 0; i < total_apresentacao; i++){
        p_atribuir_nota(ordem_apresentacao[i], (rand() % 10 + 1));
        sleep(1);
    }

    printf("\n\n\n[PROFESSOR] -> APRESENTACOES FINALIZADAS E NOTAS DISTRIBUIDAS\n\n\n");
    fim_apresentacoes = 1;
    pthread_cond_broadcast(&cond_fim_apresentacao);
    

    pthread_mutex_unlock(&regiao_critica);
} 

// Atribui nota para um aluno especifico
void p_atribuir_nota(int id_aluno, int nota){
    printf("\n\n[PROFESSOR] ->  atribuiu a nota %i para o aluno %i\n\n", nota, id_aluno);
}

// quando nao tem mais ninguem pra apresentar, termina a execução
int p_fechar_porta(){
    pthread_mutex_lock(&regiao_critica);

    if(qtde_atual_alunos_comp_sala != 0 || qtde_atual_alunos_so_sala != 0){
        printf("[PROFESSOR] ->  precisa aguardar a saida de todos os alunos para fechar a sala\n"); 
        printf("[DEBUG] qtde_atual_alunos_comp_sala -> %i ", qtde_atual_alunos_comp_sala);
        printf("[DEBUG] qtde_atual_alunos_so_sala -> %i ", qtde_atual_alunos_so_sala);
        pthread_cond_broadcast(&fim_apresentacoes);
        pthread_mutex_unlock(&regiao_critica);
        return -1;
    }

    printf("\n\n[PROFESSOR] ->  esta fechando a sala de apresentacoes\n\n");
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
    pthread_mutex_lock(&regiao_critica);

    printf("[ALUNO SO %i] -> tentando entrar na sala\n", id_aluno);

    // * Caso a sala não aceite mais apresentação(já iniciou a apresentação) 
    // * ou já está cheia, deixa o ouvinte esperando

    while(apresentacao == 1 || qtde_atual_alunos_so_sala >= num_max_alunos_so_sala){
        printf("[ALUNO SO %i] -> aguardando liberar para apresentar\n", id_aluno);
        pthread_cond_wait(&fila_alunos_so, &regiao_critica); // espera vaga na sala 
    }
    
    qtde_atual_alunos_so_sala++;
    ordem_apresentacao[num_apresentacao] = id_aluno;
    num_apresentacao++;
    pthread_cond_signal(&cond_professor_so);  // * Avisa o professor que chegou na sala para apresentar
    so_assinar_lista_entrada(id_aluno);
    
    while(apresentacao == 0){
        so_aguardar_apresentacoes(id_aluno);
        pthread_cond_wait(&cond_apresentacao, &regiao_critica);
    }
    
    //aguarda a vez de apresentar
    while(ordem_apresentacao[total_apresentacao] != id_aluno){
        so_aguardar_apresentacoes(id_aluno);
        pthread_cond_wait(&cond_apresentacao, &regiao_critica);
    }

    so_apresentar(id_aluno);
    total_apresentacao++;

    printf("[ALUNO SO %i] -> terminou apresentação\n", id_aluno);
    pthread_cond_broadcast(&cond_apresentacao);

    //aguarda apresentações terminar
    while (!fim_apresentacoes){
        printf("[ALUNO SO %i] -> aguarda para receber as notas\n", id_aluno);
        pthread_cond_wait(&cond_fim_apresentacao, &regiao_critica);
    }
    
    so_assinar_lista_saida(id_aluno);
    qtde_atual_alunos_so_sala--;
    printf("[ALUNO SO %i] -> está saindo da sala\n", id_aluno);
    pthread_mutex_unlock(&regiao_critica);
    
}
 
void so_assinar_lista_entrada(int id_aluno){ // dentro do so_entrar_sala

    printf("[ALUNO SO %i] -> assinou a lista de entrada\n", id_aluno);

}

void so_aguardar_apresentacoes(int id_aluno){ // quando o aluno não conseguir apresentar, fica esperando

    printf("[ALUNO SO %i] -> aguarda apresentação\n\n", id_aluno);

} 

void so_apresentar(int id_aluno){ // aluno de SO apresenta

    printf("[ALUNO SO %i] -> inicia a apresentação\n\n", id_aluno);
    sleep(1);

}

void so_assinar_lista_saida(int id_aluno){ // tem que assinar quando for sair da sala

    printf("[ALUNO SO %i] ->  assinou a lista de saída\n", id_aluno);

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

    printf("[ALUNO COMP. %i] -> tentando entrar na sala\n", id_aluno);

    // * Caso a sala não aceite mais ouvintes(já iniciou a apresentação) 
    // * ou já está cheia, deixa o ouvinte esperando
    while(apresentacao == 1  || qtde_atual_alunos_comp_sala >= num_max_alunos_comp_sala){
        printf("[ALUNO COMP. %i] -> aguardando a sala liberar para ouvintes\n\n", id_aluno);
        pthread_cond_wait(&fila_alunos_comp, &regiao_critica); // espera ate que tenha vagas na sala
    }

    qtde_atual_alunos_comp_sala++;
    pthread_cond_signal(&cond_professor_so); // * Avisa o professor que chegou para assistir a apresentação
    printf("[ALUNO COMP. %i] -> entrou na sala e esta esperando o inicio da apresentacao\n\n", id_aluno);

    // * enquanto não iniciar a apresentação, o ouvinte espera
    while (apresentacao == 0){
        pthread_cond_wait(&cond_apresentacao, &regiao_critica);
    }
    comp_assistir_apresentacao(id_aluno);

    // TODO: meio de o aluno poder sair durante a apresentação 

    int aux =  rand() % 10 + 1;
    if(aux != 5){
        while(fim_apresentacoes == 0){
            printf("\n[ALUNO COMP. %i] -> vai esperar ate o final\n", id_aluno);
            pthread_cond_wait(&cond_fim_apresentacao, &regiao_critica);
        }
    } 
    
    comp_sair_apresentacao(id_aluno);    
    
    pthread_mutex_unlock(&regiao_critica); // liberando o mutex
}

/*
 * Metodo que define o comportamento do aluno de computacao enquanto assiste uma apresentação 
 * Parametros:
 * int id_aluno -> id do aluno para melhor visualizacao
 */
void comp_assistir_apresentacao(int id_aluno){
    printf("\n[ALUNO COMP. %i] -> esta assistindo a apresentacao\n\n", id_aluno);
} 

/*
 * Metodo que define o comportamento do aluno de computacao ao sair da apresentação
 * Parametros:
 * int id_aluno -> id do aluno para melhor visualizacao
 */
void comp_sair_apresentacao(int id_aluno){
    printf("\n[ALUNO COMP. %i] -> saiu da apresentacao\n\n", id_aluno);
    qtde_atual_alunos_comp_sala--;
    sleep(1);
}

// -------------------- Fim Metodos dos alunos de computação ---------------------
