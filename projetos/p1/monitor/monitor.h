#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t regiao_critica; // mutex para proteger os metodos executados
pthread_mutex_t sala; // mutex para representar a sala
// pthread_mutex_t apresentacao; // mutex para quem vai fazer a apresentação


pthread_cond_t fila_alunos_comp; // variave de condicao para os alunos de computacao
pthread_cond_t fila_alunos_so; //variavel de condicao para os alunos de SO
pthread_cond_t cond_professor_so; //variavel de condicao para o professor
pthread_cond_t cond_apresentacao; //variavel de condicao para todos que estao esperando o inicio das apresentacoes
pthread_cond_t cond_fim_apresentacao; //variavel de condicao para o fim das apresentacões


int num_max_alunos_comp_sala;
int num_min_alunos_comp_sala;
int num_max_alunos_so_sala;
int qtde_atual_alunos_comp_sala;
int qtde_atual_alunos_so_sala;
int apresentacao; // boolean para definir se esta acontecendo alguma apresentacao
int aceitaOuvintes; // boolean para definir se a apresentacao ainda aceita ouvintes
int ordem_apresentacao[2]; // controla ordem de quem vai apresentar
int num_apresentacao; // defini a posição de apresentação
int total_apresentacao; // mostra quantas apresentações foram feitas

// Métodos do monitor
void init_monitor();
void destroy_monitor();

// métodos do professor
void p_iniciar_apresentacoes(); // tem que acordar 1 aluno de so e 5 alunos de computação

// libera a entrada dos alunos, somente se a sala estiver vazia
// caso nao esteja, retorna -1
// caso tudo ocorra normalmente e o prof consiga liberar a entrada, retorna 1
int p_liberar_entrada(); 

void p_atribuir_nota();

// fecha a sala, somente se estiver vazia
// caso nao esteja, retorna -1
// caso tudo ocorra normalmente e o prof consiga liberar a entrada, retorna 1
int p_fechar_porta(); 

//métodos dos alunos de SO
void so_entrar_sala(int id_aluno); // tem que entrar na fila_alunos_so
void so_assinar_lista_entrada(int id_aluno); // dentro do so_entrar_sala
void so_aguardar_apresentacoes(int id_aluno); // quando o aluno não conseguir apresentar, fica esperando
void so_apresentar(int id_aluno); // aluno de SO apresenta
void so_assinar_lista_saida(int id_aluno); // tem que assinar quando for sair da sala

// Metodos dos alunos de computação
void comp_entrar_sala(int id_aluno); // tem que esperar na fila_alunos_comp
void comp_assistir_apresentacao(int id_aluno); // metodo pra somente assistir, 
void comp_sair_apresentacao(int id_aluno); // pro aluno sair da apresentação (ps: pode sair no meio da apresentação)
