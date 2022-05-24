#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t regiao_critica; // mutex para proteger os metodos executados
pthread_mutex_t sala; // mutex para representar a sala
// pthread_mutex_t apresentacao; // mutex para quem vai fazer a apresentação


pthread_cond_t fila_alunos_comp; // variave de condicao para os alunos de computacao
pthread_cond_t fila_alunos_so; //variavel de condicao para os alunos de SO
pthread_cond_t cond_professor; //variavel de condicao para o professor
pthread_cond_t cond_apresentacao; //variavel de condicao para todos que estao esperando o inicio das apresentacoes



int num_max_alunos_comp_sala;
int num_min_alunos_comp_sala;
int num_min_alunos_so_sala;
int qtde_atual_alunos_sala;
int apresentacao; // boolean para definir se esta acontecendo alguma apresentacao
int aceitaOuvintes; // boolean para definir se a apresentacao ainda aceita ouvintes


// Métodos do monitor
void init_monitor();
void destroy_monitor();

// métodos do professor
void p_iniciar_apresentacoes(); // tem que acordar 1 aluno de so e 5 alunos de computação
void p_liberar_entrada(); // dispara o for que vai acordar os alunos de computacao e de SO
void p_atribuir_nota();
void p_fechar_porta(); // quando nao tem mais ninguem pra apresentar, termina a execução

//métodos dos alunos de SO
void so_entrar_sala(); // tem que entrar na fila_alunos_so
void so_assinar_lista_entrada(); // dentro do so_entrar_sala
void so_aguardar_apresentacoes(); // quando o aluno não conseguir apresentar, fica esperando
void so_apresentar(); // aluno de SO apresenta
void so_assinar_lista_saida(); // tem que assinar quando for sair da sala

// Metodos dos alunos de computação
void comp_entrar_sala(); // tem que esperar na fila_alunos_comp
void comp_assistir_apresentacao(); // metodo pra somente assistir, 
void comp_sair_apresentacao(); // pro aluno sair da apresentação (ps: pode sair no meio da apresentação)
