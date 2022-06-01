/*
    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 31/05/2022
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t regiao_critica; // mutex para proteger os metodos executados

pthread_cond_t fila_alunos_comp; // variave de condicao para os alunos de computacao
pthread_cond_t fila_alunos_so; //variavel de condicao para os alunos de SO
pthread_cond_t cond_professor_so; //variavel de condicao para o professor
pthread_cond_t cond_apresentacao; //variavel de condicao para todos que estao esperando o inicio das apresentacoes
pthread_cond_t cond_fim_apresentacao; //variavel de condicao para o fim das apresentacões


int num_max_alunos_comp_sala;
int num_max_alunos_so_sala;
int qtde_atual_alunos_comp_sala;
int qtde_atual_alunos_so_sala;
int apresentacao; // boolean para definir se esta acontecendo alguma apresentacao
int aceitaOuvintes; // boolean para definir se a apresentacao ainda aceita ouvintes
int ordem_apresentacao[2]; // controla ordem de quem vai apresentar
int num_apresentacao; // defini a posição de apresentação
int total_apresentacao; // mostra quantas apresentações foram feitas
int fim_apresentacoes; // booleano que define se ocorreu o fim de todas as apresentacoes

// =================== Métodos do monitor ===================
void init_monitor();
void destroy_monitor();

// =================== Métodos do professor ===================

// professor tenta iniciar as apresentacoes, caso ainda nao tenha a quantidade necessaria de alunos
// espera ate que eles cheguem
void p_iniciar_apresentacoes(); 

// libera a entrada dos alunos, somente se a sala estiver vazia
// caso nao esteja, retorna -1
// caso tudo ocorra normalmente e o prof consiga liberar a entrada, retorna 1
int p_liberar_entrada(); 

// atribui a nota para um aluno especifico
// parametro: int id_aluno -> id que representa o aluno
// parametro: int nota -> nota do aluno
void p_atribuir_nota(int id_aluno, int nota);

// fecha a sala, somente se estiver vazia
// caso nao esteja, retorna -1
// caso tudo ocorra normalmente e o prof consiga liberar a entrada, retorna 1
int p_fechar_porta(); 

// =================== Métodos dos alunos de SO ===================

// o aluno de so tenta entrar na sala, caso nao consiga, fica esperando ate ter uma vaga
// parametro: int id_aluno -> id que representa o aluno
void so_entrar_sala(int id_aluno); 

// o aluno de so assina a lista logo apos entrar na sala
// parametro: int id_aluno -> id que representa o aluno
void so_assinar_lista_entrada(int id_aluno); 

// aluno espera as apresentacoes para sair da sala
// parametro: int id_aluno -> id que representa o aluno
void so_aguardar_apresentacoes(int id_aluno); 

// aluno apresenta o trabalho
// parametro: int id_aluno -> id que representa o aluno
void so_apresentar(int id_aluno); 

// aluno assina a lista na saida da sala 
// parametro: int id_aluno -> id que representa o aluno
void so_assinar_lista_saida(int id_aluno); 

// =================== Metodos dos alunos de computação ===================

// aluno de comp tenta entrar na sala, caso nao consiga fica esperando vaga
// parametro: int id_aluno -> id que representa o aluno
void comp_entrar_sala(int id_aluno);

// aluno de comp assiste a apresentacao
// parametro: int id_aluno -> id que representa o aluno
void comp_assistir_apresentacao(int id_aluno); 

// momento em que o aluno de comp sai da sala
// parametro: int id_aluno -> id que representa o aluno
void comp_sair_apresentacao(int id_aluno); 
