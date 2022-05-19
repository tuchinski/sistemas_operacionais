#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

/*
    Implemente um programa que realize a soma de vetores utilizando processos para fazer o cálculo, mas com os vetores sendo compartilhados pelos processos. Como os espaços de memória entre os processos são 
    isolados, um mecanismo fornecido pelo SO deve ser usado. No caso, use memória compartilhada para que todos os filhos operem sobre os dados, e pipes para a realização do despacho de 
    trabalho (intervalo de índices no vetor). O número de elementos do vetor e o número de processos filhos deve ser fornecido pelo usuário. Por exemplo, numElementos = 1000 e numProcessos = 5, cada filho 
    processará 200 índices; para numElementos = 1000 e numProcessos = 4, cada filho processará 250 índices.

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 23/04/2022

*/

int count, shmsz;

key_t key = 3254;

// funcão que é executada pelos filhos criados
int child(int* nome_pipe){
    int buffer_lido[6];
    close(nome_pipe[1]);
    read(nome_pipe[0], buffer_lido, sizeof(int) * 6);

    int memory_id_filho, *memory_filho, *share_filho;

    if ((memory_id_filho = shmget(key, shmsz, 0666)) < 0){
        printf("Erro ao tentar acessar o segmento do Filho %i.", count); 
        exit(1); 
    }
    if ((memory_filho = shmat(memory_id_filho, NULL, 0)) == (int*)-1){ 
        perror("Erro ao acoplar o segmento ao espaço de dados do programa."); 
        exit(1); 
    }

    share_filho = memory_filho;
    int index_vetor_1 = buffer_lido[0], index_vetor_2 = buffer_lido[2], index_vetor_resultado = buffer_lido[4];

    // realiza a soma dos vetores
    while(index_vetor_1 < buffer_lido[1]) {
        memory_filho[index_vetor_resultado] = share_filho[index_vetor_1] + share_filho[index_vetor_2];
        index_vetor_1++; index_vetor_2++; index_vetor_resultado++;
    }

    if (shmdt(memory_filho) == -1){ 
        perror("Erro ao desacoplar da região de memória compartilhada."); 
        exit(1); 
    }
    fflush(stdout);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
	count = 0; 
    int index, memory_id_pai, *memory_pai, *share_pai, qtde_elementos, processos,min_range, mid_range, max_range, intervalo;
    
    
    if(argc != 3){
        fprintf(stderr, "Uso: nome_do_arquivo <qtde de elementos> <num de processos>");
        exit(0);
    }

    if(atoi(argv[1]) <= 0){
        fprintf(stderr, "%d precisa ser maior que 0", atoi(argv[1]));
    }
    if(atoi(argv[2]) <= 0){
        fprintf(stderr, "%d precisa ser maior que 0", atoi(argv[2]));
    }

    qtde_elementos = atoi(argv[1]);
    processos = atoi(argv[2]); 
    shmsz = (qtde_elementos * 3) * 4;
    min_range = 0;
    mid_range = qtde_elementos;
    max_range = qtde_elementos * 2;


    // cria o espaço da memória compartilhada com o espaco de TAM_VET * 3
    if ((memory_id_pai = shmget(key, shmsz, IPC_CREAT | 0666)) < 0) { 
        perror("Erro ao tentar criar o segmento de shm."); 
        exit(1); 
    }

    if ((memory_pai = shmat(memory_id_pai, NULL, 0)) == (int*)-1) {
        perror("Erro ao acoplar o segmento ao espaço de dados do programa."); 
        exit(1); 
    }
    printf("\nREGIÃO DE MEMÓRIA (%d)\n\n", memory_id_pai);

    share_pai = memory_pai;

    srand((unsigned)time(NULL));
    
    // colocando valores nos vetores 1 e 2
    for(index = min_range; index < max_range; index++) {
        share_pai[index] = rand()%10;
    }

    printf("-VETOR[1]: |");
    for(index = min_range; index < mid_range; index++){
        printf("%d|", share_pai[index]);
    }

    printf("\n");

    printf("-VETOR[2]: |");
    for(index = mid_range; index < max_range; index++) printf("%d|", share_pai[index]);
    printf("\n\n");

    int pipes[processos][2];

    int min = 0;
    int mid = qtde_elementos;
    int max = qtde_elementos*2;

    // cria os pipes que vao armazenar as informações para cada filho que fará o processamento
    intervalo = qtde_elementos/processos;
    for (int i = 0; i < processos; i++)
    {
        if(pipe(pipes[i]) < 0 ){
            printf(" erro");
        }
    }

    
    for(int i = 0; i < processos; i++){
        int buffer_local[6];
        buffer_local[0] = min; // onde comeca a contar no vet1 
        buffer_local[1] = min + intervalo; // onde termina a contar no vet1 
        buffer_local[2] = mid; // onde comeca a contar no vet2
        buffer_local[3] = mid + intervalo; // onde termina a contar no vet2
        buffer_local[4] = max; // onde comeca no vetor de resultado
        buffer_local[5] = max + intervalo; // onde termina no vetor de resultado

        if(i == processos -1){
            buffer_local[1] = qtde_elementos;
            buffer_local[3] = qtde_elementos*2;
            buffer_local[5] = qtde_elementos*3;
        }
        write(pipes[i][1], buffer_local, sizeof(buffer_local)); // escreve nos pipes
        

        min += intervalo;
        mid += intervalo;
        max += intervalo;
    }

    // cria os n filhos para fazer o processamento
    for (int i = 0; i < processos; i++)
    {
        pid_t filho;
        count++;
        if((filho = fork()) == 0){
            child(pipes[i]);
            exit(1);
        } 
    }


    pid_t pidFilho;
    int status;
    do {
        pidFilho = wait(&status);
        printf("--Filho %d finalizaou com status %d\n", pidFilho, status);
    } while (pidFilho > 0);


    // wait(NULL); // espera os filhos

    printf("RESULTADO: |");
    for (int j = qtde_elementos*2; j < qtde_elementos*2 + qtde_elementos; j++){
        printf("%d|", memory_pai[j]);
    }
    printf("\n\n");

    if (shmdt(memory_pai) == -1){ 
        perror("Erro ao desacoplar da região de memória compartilhada."); 
        exit(1); 
    }
    if (shmctl(memory_id_pai, IPC_RMID, 0) == -1){ 
        perror("Erro ao destruir o segmento compartilhado. (shmctl)."); 
        exit(1); 
    }
    
    return EXIT_SUCCESS;
    
}