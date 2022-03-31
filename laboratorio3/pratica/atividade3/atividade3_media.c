#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 4

/*
    Implemente um programa multithread com pthreads que calcule:
    b) a média aritmética de cada coluna de uma matriz MxN e devolva o resultado em um vetor de
        tamanho N.
    O programa deve gerar matrizes MxN com elementos aleatórios para arquivos; usar técnicas de
    paralelização de funções e de dados; ler matrizes MxN de arquivos no formato em anexo; gravar os
    resultados em um arquivo texto.
*/

struct data_chunk
{
    int** matriz;
    int pos_inicial;
    int pos_final;
    int num_total_linhas;
    int num_total_colunas;
    int* resultado;
};

void imprime_matriz(int** matriz, int qtde_linhas, int qtde_colunas){
    for (int i = 0; i < qtde_linhas; i++)
    {
        for (int j = 0; j < qtde_colunas; j++)
        {
            printf(" %i ", matriz[i][j]);
        }
        printf("\n");
    }
}

void* calcula_media_coluna(void* param){
    struct data_chunk* dados = param;
    int soma = 0;

    for (int c = dados->pos_inicial; c < dados->pos_final; c++)
    {
        for (int l = 0; l < dados->num_total_linhas; l++)
        {
            soma = soma + dados->matriz[l][c];
        }
        dados->resultado[c] = soma;
        soma = 0;
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    // char nomearquivo[] = "matriz_6por8.in";
    
    FILE *file = fopen(argv[1], "r");
    if(!file){
        printf("Erro ao abrir o arquivo. Verificar se o nome do arquivo digitado está correto");
        exit(1);
    }
    char c;
    char num_atual[4];
    int a = 0;// acumulador pra caso os numeros tenham mais de um digito

    int dimensoes[2]; // guarda as dimensoes da matriz do arquivo
    int d;
    
    int linha_atual = 0;
    int col_atual = 0;

    while ((c = getc(file)) != '\n'){
        if(c != 'x' && c != 'X'){
            num_atual[a] = c;
            a++;
        }else{
            dimensoes[d] = atoi(num_atual);
            for (int i = 0; i < 4; i++)
            {
                num_atual[i] = '\0';
            }
            
            d += 1;
            a = 0;
        }   
    }
    dimensoes[d] = atoi(num_atual);
    for (int i = 0; i < 4; i++)
    {
        num_atual[i] = '\0';
    }
    a = 0;
    printf("Linhas: %i\n", dimensoes[0]);
    printf("Colunas: %i\n\n", dimensoes[1]);


    
    // aloca um vetor de qtde_linhas ponteiros para linhas
    int** matriz = malloc (dimensoes[0] * sizeof (int*)) ;

    // aloca cada uma das linhas (vetores de qtde_col inteiros)
    for (int i = 0; i < dimensoes[0]; i++)
       matriz[i] = malloc (dimensoes[1] * sizeof (int)) ;

    // int matriz[qtde_linhas][qtde_colunas];

    


    // le a matriz do documento
    while ((c = getc(file)) != EOF){
        if(c == '\n'){
            matriz[linha_atual][col_atual] = atoi(num_atual);
            for (int i = 0; i < 4; i++)
            {
                num_atual[i] = '\0';
            }
            linha_atual += 1;
            col_atual = 0;
            a = 0;
            continue;
        }

       if(c != ' '){
            num_atual[a] = c;
            a++;
        }else{
            matriz[linha_atual][col_atual] = atoi(num_atual);
            for (int i = 0; i < 4; i++)
            {
                num_atual[i] = '\0';
            }
            
            col_atual += 1;
            a = 0;
        }   
    }

    matriz[linha_atual][col_atual] = atoi(num_atual); // necessario pra ler o ultimo numero da matriz
    
    fclose(file);
    imprime_matriz(matriz, dimensoes[0], dimensoes[1]);

    int* resultado_media = (int*) malloc(sizeof(int) * dimensoes[1]);

    struct data_chunk dados[NUM_THREADS];

    int tam_parte = dimensoes[1]/NUM_THREADS;\
    int parte_inicial = 0;
    int parte_final = tam_parte;
    for (int p = 0; p < NUM_THREADS; p++)
    {
        dados[p].matriz = matriz;
        dados[p].num_total_linhas = dimensoes[0];
        dados[p].num_total_colunas = dimensoes[1];
        dados[p].resultado = resultado_media;
        dados[p].pos_inicial = parte_inicial;
        dados[p].pos_final = parte_final;
    }
    dados[NUM_THREADS-1].pos_final = dimensoes[1];

    
    pthread_t threads[NUM_THREADS];
    for (int p = 0; p < NUM_THREADS; p++)
    {
        printf("\nCriando Thread [%i]", p);
        pthread_create(&threads[p], NULL, calcula_media_coluna, &dados[p]);
    }

     for (int p=0; p < NUM_THREADS; p++){
        pthread_join (threads[p], NULL);
     }
    
    printf("\n\nsaindo a soma:\n\n\n");
    for (int i = 0; i < dimensoes[1]; i++)
    {
        printf(" %i ", resultado_media[i]);
    }

    return 0;
}
