//OBS: A quantidade de alunos de SO é determinado pelo usuário
//OBS: A quantidade de alunos de computação é determinado seguindo a formula (numAlunosSO / 2) * 5
//OBS: Pois, cada rodada de apresentação terá dois alunos de SO apresentando, e 5 alunos de computação na plateia
//OBS: Para simular a decisão de sair da sala durante a apresentação, foi utilizando a função rand, com numero de 1 a 10
//OBS: Onde o sorteio do numero 5 marca a decisão de sair da sala, ou seja, quando for sorteado o numero 5, o aluno sairá da sala
//OBS: se não, ele assistirá a apresentação.

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//semaforo utilizado pelos alunos de SO antes de entrar na sala
sem_t SemAlunosSo;
//semaforo utilizado pelos alunos de computação antes de entrar na sala
sem_t SemAlunosComputacao;
//semaforo utilizado pelos alunos de SO após de entrar na sala
sem_t SemApresentacao;
//semaforo utilizado pelos alunos de computação após de entrar na sala
sem_t SemPlateia;
//semaforo utilizado pelo professor
sem_t SemProfessor;

//quantidade de alunos de SO criados
int alunosSoCriados = 0;
//quantidade de alunos de computacao criados
int alunosComputacaoCriados = 0;
//quantidade de alunos de SO informado pelo usuário
int numTotalAlunosSo = 0;
//quantidade de alunos de computacao será
int numTotalAlunosComputacao = 0;
//numero de alunos na plateia
int numPlateia = 0;
//ordem da apresentação
int numApresentacao = 0;
bool fimApresentacao = false;
int totalApresentacoes = 0;
int numAlunosSORodada = 0;
int ordemApresentacao[2] = {-1, -1};

void iniciarApresentacao();
void liberarEntrada();
void atribuirNota(int id, int nota);
void fecharPorta();
void entrarSala(int id, int tipoALuno);
void assinarListaEntrada(int id);
void aguardarApresentacoes(int id, bool apresentou);
void apresentar(int id);
void assinarListaSaida(int id);
void assistirApresentacoes(int id);
void sairSala(int id);

void *funcProfessor(void *ptr)
{
    printf("O professor foi criado\n");
    while (totalApresentacoes < numTotalAlunosSo)
    {
        while (alunosSoCriados < numTotalAlunosSo || alunosComputacaoCriados < numTotalAlunosComputacao)
        {
            continue;
        }
        fimApresentacao = 0;
        liberarEntrada();
        int i = 0;
        //Aguarda a entrada da plateia
        for (i = 0; i < 5; i++)
        {
            sem_post(&SemAlunosComputacao);
            sem_wait(&SemProfessor);
        }
        numPlateia = 5;
        //define quantos alunos de SO iram apresentar na rodada e Aguarda a entrada dos alunos de so
        if(numTotalAlunosSo - totalApresentacoes == 1){
            numAlunosSORodada = 1;
        }
        else{
            numAlunosSORodada = 2;
        }
        for (i = 0; i < numAlunosSORodada; i++){
            sem_post(&SemAlunosSo);
            sem_wait(&SemProfessor);
        }
        
        fecharPorta();
        //Aguarda a assinatura da lista de entrada
        for (i = 0; i < numAlunosSORodada; i++)
        {
            sem_post(&SemApresentacao);
            sem_wait(&SemProfessor);
        }
        iniciarApresentacao();
        //Aguarda todas as apresentações
        for (i = 0; i < numAlunosSORodada; i++)
        {
            sem_post(&SemApresentacao);
            sem_wait(&SemProfessor);
        }
        totalApresentacoes += 2;
        //Atribui nota a todos os alunos
        for (i = 0; i < numAlunosSORodada; i++)
        {
            atribuirNota(ordemApresentacao[i], (rand() % 10 + 1));
        }
        //Aguarda a saida dos alunos de so
        for (i = 0; i < numAlunosSORodada; i++)
        {
            sem_post(&SemApresentacao);
            sem_wait(&SemProfessor);
        }
        //Aguarda a saida dos alunos de computacao
        fimApresentacao = 1;
        for (i = 0; i < numPlateia; i++)
        {
            sem_post(&SemPlateia);
            sem_wait(&SemProfessor);
        }
        numApresentacao = 0;
        printf("\nFim da rodada de apresentações\n\n");
    }

    fecharPorta();
    printf("\n APRESENTAÇÕES ENCERRADAS!!!\n");
    return 0;
}

void *funcAlunosSo(void *ptr)
{
    int id = (intptr_t)ptr;
    alunosSoCriados++;
    //aguarda para entrar na sala
    sem_wait(&SemAlunosSo);
    entrarSala(id, 0);
    sem_post(&SemProfessor);
    //aguarda para assinar a lista de entrada e aguarda para apresentar
    sem_wait(&SemApresentacao);
    assinaListaEntrada(id);
    sem_post(&SemProfessor);
    sem_wait(&SemApresentacao);
    //apresenta
    apresentar(id);
    sem_post(&SemProfessor);
    //aguarda o fim das apresentações
    aguardarApresentacoes(id, 1);
    sem_wait(&SemApresentacao);
    assinaListaSaida(id);
    sem_post(&SemProfessor);
    return 0;
}

void *funcAlunosComputacao(void *ptr)
{
    int id = (intptr_t)ptr;
    alunosComputacaoCriados++;
    sem_wait(&SemAlunosComputacao);
    entrarSala(id, 1);
    sem_post(&SemProfessor);
    sem_wait(&SemPlateia);
    int aux = -1;
    while (!fimApresentacao)
    {
        aux = rand() % 10 + 1;
        if (aux != 5)
        {
            assistirApresentacao(id);
            sem_wait(&SemPlateia);
        }
        else
        {
            sairApresentacao(id);
            numPlateia--;
            return 0;
        }
    }
    sairApresentacao(id);
    sem_post(&SemProfessor);
    return 0;
}

void entrarSala(int id, int tipoAluno)
{
    if (tipoAluno == 0)
    {
        printf("O aluno %d de SO entrou na sala para apresentar\n", id);
        sleep(2);
    }
    else
    {
        printf("O aluno %d de computacao entrou na sala para assistir\n", id);
        sleep(2);
    }
}

void assinaListaEntrada(int id)
{
    printf("O aluno %d de SO assinou a lista de entrada\n", id);
    aguardarApresentacoes(id, 0);
    sleep(2);
}

void assinaListaSaida(int id)
{
    printf("O aluno %d de SO assinou a lista de saida e deixou a sala\n", id);
    sleep(2);
}

void apresentar(int id)
{
    printf("O aluno %d de SO está apresentando\n", id);
    ordemApresentacao[numApresentacao] = id;
    numApresentacao++;
    int i = 0;
    for (i = 0; i < numPlateia; i++)
    {
        sem_post(&SemPlateia);
    }
    sleep(10);
}

void aguardarApresentacoes(int id, bool apresentou){
    if(apresentou){
        printf("O aluno %d de SO já apresentou e agora está aguardando\n\n", id);
    }
    else{
        printf("O aluno %d de SO está aguardando a sua vez de apresentar\n", id);
    }
}

void assistirApresentacao(int id)
{
    printf("O aluno %d de computação está assistindo a apresentação\n", id);
    sleep(1);
}

void sairApresentacao(int id)
{
    printf("O aluno %d de computação deixou a sala\n", id);
}

void iniciarApresentacao()
{
    printf("\nO professor iniciou as apresentações\n\n");
    sleep(2);
}

void liberarEntrada()
{
    printf("O professor liberou a entrada na sala\n");
    sleep(2);
}

void atribuirNota(int id, int nota)
{
    printf("O professor atribuiu a nota %d ao aluno %d de SO\n", nota, id);
    sleep(2);
}

void fecharPorta()
{
    printf("O professor fechou a porta da sala\n");
    sleep(2);
}

int defineNumAlunosComputacao(int alunosSo){
    if(alunosSo % 2 == 0){
        return alunosSo / 2;
    }
    else{
        return (alunosSo / 2) + 1;
    }
}

int main(void)
{

    //inicializa os semaforos
    sem_init(&SemAlunosSo, 0, 0);
    sem_init(&SemAlunosComputacao, 0, 0);
    sem_init(&SemProfessor, 0, 0);
    sem_init(&SemPlateia, 0, 0);
    sem_init(&SemApresentacao, 0, 0);

    //pergunta a quantidade de alunos de SO
    printf("\nDigite a quantidade de ALUNOS DE SO: ");
    scanf("%d", &numTotalAlunosSo);
    while(numTotalAlunosSo <= 0){
        printf("\nA quantidade de alunos de SO deve ser maior ou igual a 1\n\nDigite a quantidade de ALUNOS DE SO: ");
        scanf("%d", &numTotalAlunosSo);  
    }
    pthread_t ThreadAlunosSo[numTotalAlunosSo];
    pthread_t ThreadAlunosComputacao[(numTotalAlunosSo / 2) * 5];

    //cria os alunos de SO e de computação
    int i = 0;
    for (i = 0; i < numTotalAlunosSo; i++)
    {
        pthread_create(&ThreadAlunosSo[i], NULL, funcAlunosSo, (void *)(intptr_t)i);
    }
    for (i = 0; i < defineNumAlunosComputacao(numTotalAlunosSo) * 5; i++)
    {
        pthread_create(&ThreadAlunosComputacao[i], NULL, funcAlunosComputacao, (void *)(intptr_t)i);
    }

    //cria o professor
    pthread_t ThreadProfessor;
    pthread_create(&ThreadProfessor, NULL, funcProfessor, NULL);

    for (i = 0; i < numTotalAlunosSo; i++)
    {
        pthread_join(ThreadAlunosSo[i], NULL);
    }

    pthread_join(ThreadProfessor, NULL);

    //destroi os semaforos no final
    sem_destroy(&SemAlunosComputacao);
    sem_destroy(&SemAlunosSo);
    sem_destroy(&SemPlateia);
    sem_destroy(&SemApresentacao);
    sem_destroy(&SemProfessor);

    return 0;
}