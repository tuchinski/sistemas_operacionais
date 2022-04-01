#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define NUM_THREADS 6

/*
    Implemente um programa multithread com pthreads que calcule:
    a) a mediana de cada linha de uma matriz MxN e devolva o resultado em um vetor de tamanho M.
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
    float* resultado;
};

void cria_matriz_random(int linhas, int colunas){
    char nome_arquivo[13] = "matriz_gerada";
    FILE* arq;
    srand(time(NULL));
    
    if((arq = fopen(nome_arquivo, "wb")) != NULL){
        fprintf(arq, "%ix%i\n", linhas, colunas);
        for (int i = 0; i < linhas; i++){
            for (int j = 0; j < colunas; j++){
                fprintf(arq, "%i",rand() % 100);
                if(j != linhas-1){
                    fprintf(arq, " ");    
                }
            }
            if(i != linhas-1){
                fprintf(arq, "\n");
            }
        }
    }  
    fclose(arq);
}

void escreve_resultado_arquivo(char* nome_arquivo, float* resultado_media, float* resultado_mediana, int tam_media, int tam_mediana){
    FILE* arq;

    
    if((arq = fopen(nome_arquivo, "w")) != NULL){
        fprintf(arq, "Resultado do calculo da mediana:\n");
        for (int i = 0; i < tam_mediana; i++)
        {
            fprintf(arq, "Linha [%i] -> %0.2f\n", i, resultado_mediana[i]);
        }
        
        fprintf(arq, "\n\n");

        fprintf(arq, "Resultado do calculo da media:\n");
        for (int i = 0; i < tam_media; i++)
        {
            fprintf(arq, "Coluna [%i] -> %0.2f\n", i, resultado_media[i]);
        }
        
    }  
    fclose(arq);
}

void imprime_matriz(int** matriz, int qtde_linhas, int qtde_colunas){
    for (int i = 0; i < qtde_linhas; i++)
    {
        printf("|");
        for (int j = 0; j < qtde_colunas; j++)
        {
            printf(" %i ", matriz[i][j]);
            printf("|");
        }
        printf("\n");
    }
}

void* calcula_mediana_linha(void* param){
    struct data_chunk* dados = param;

    for (int linha = dados->pos_inicial; linha < dados->pos_final; linha++){
        
        int indice_mediana = dados->num_total_colunas/2;
        float valor_mediana = dados->matriz[linha][indice_mediana];
        
        if (dados->num_total_colunas%2 == 0){
            valor_mediana = (float)(dados->matriz[linha][indice_mediana] + dados->matriz[linha][indice_mediana-1])/2;
        } 
        dados->resultado[linha] = (float)valor_mediana;    
    }
    return NULL;
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
        dados->resultado[c] = (float)soma/dados->num_total_linhas;
        soma = 0;
    }
    return NULL;
}

int main(int argc, char const *argv[])
{    char arquivo[60] = "matriz_gerada";
    // Caso nao seja digitado nenhum arquivo, o programa gera uma matriz com dados aleatorios
    if(argv[1] == NULL){
        char entrada;
        printf("Nenhum arquivo digitado. Pressione qualquer tecla para gerar matriz aleatoria ou '0' para Sair:\n");
        scanf("%c", &entrada);
        if(entrada == '0'){
            exit(0);
        }else{
            int linhas, colunas;
            printf("Digite o numero de linhas da matriz:");
            scanf("%i", &linhas);
            printf("Digite o numero de colunas da matriz:");
            scanf("%i", &colunas);
            cria_matriz_random(linhas, colunas);
        }
    }else{
        strcpy(arquivo, argv[1]);
        printf("dasdsadsdateste\n\n\n");
    }

    // ------------------- Leitura do arquivo -----------------------------------
    FILE *file = fopen(arquivo, "r");
    if(!file){
        printf("Erro ao abrir o arquivo. Verificar se o nome do arquivo digitado está correto\n");
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
    
    // aloca um vetor de qtde_linhas ponteiros para linhas
    int** matriz = calloc (dimensoes[0], sizeof (int*)) ;

    // aloca cada uma das linhas (vetores de qtde_col inteiros)
    for (int i = 0; i < dimensoes[0]; i++)
       matriz[i] = calloc (dimensoes[1], sizeof (int)) ;

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
    printf("Matriz\n");
    matriz[linha_atual][col_atual] = atoi(num_atual); // necessario pra ler o ultimo numero da matriz
    
    fclose(file);
    // ------------------- Fim da leitura do arquivo -----------------------------------

    // imprime_matriz(matriz, dimensoes[0], dimensoes[1]);

    // Vetor que vai guardar o resultado da media das colunas
    float* resultado_media = (float*) calloc(dimensoes[1], sizeof(float));

    // Vetor que vai guardar o resultado da media das colunas
    float* resultado_mediana = (float*) calloc(dimensoes[0], sizeof(float));

    // dividindo a qtde de threads para executar as funcoes paralelamente
    int num_threads_media = 1;
    int num_threads_mediana = 1;
    if(NUM_THREADS > 1){
        num_threads_media = NUM_THREADS / 2;
        num_threads_mediana = NUM_THREADS - num_threads_media;
    }

    printf("\nThreads para media: %i\n", num_threads_media);
    printf("Threads para mediana: %i\n", num_threads_mediana);

    // Criando as estruturas de dados para calculo da media
    struct data_chunk dados_media[num_threads_media];
    int tam_parte = dimensoes[1]/num_threads_media;
    int parte_inicial = 0;
    int parte_final = tam_parte;
    for (int p = 0; p < num_threads_media; p++)
    {
        dados_media[p].matriz = matriz;
        dados_media[p].num_total_linhas = dimensoes[0];
        dados_media[p].num_total_colunas = dimensoes[1];
        dados_media[p].resultado = resultado_media;
        dados_media[p].pos_inicial = parte_inicial;
        dados_media[p].pos_final = parte_final;
    }
    dados_media[num_threads_media-1].pos_final = dimensoes[1];  
    
    // Definindo as estruturas de dados para calculo da mediana
    struct data_chunk dados_mediana[num_threads_mediana];
    tam_parte = dimensoes[0]/num_threads_media;
    parte_inicial = 0;
    parte_final = tam_parte;
    for (int p = 0; p < num_threads_mediana; p++)
    {
        dados_mediana[p].matriz = matriz;
        dados_mediana[p].num_total_linhas = dimensoes[0];
        dados_mediana[p].num_total_colunas = dimensoes[1];
        dados_mediana[p].resultado = resultado_mediana;
        dados_mediana[p].pos_inicial = parte_inicial;
        dados_mediana[p].pos_final = parte_final;
    }
    dados_mediana[num_threads_mediana-1].pos_final = dimensoes[0];


    // Criando as threads para calculo da media
    pthread_t threads_media[num_threads_media];
    for (int p = 0; p < num_threads_media; p++)
    {
        printf("\nCriando Thread media [%i]", p);
        pthread_create(&threads_media[p], NULL, calcula_media_coluna, &dados_media[p]);
    }

     // Criando as threads para calculo da mediana
    pthread_t threads_mediana[num_threads_mediana];
    for (int t = 0; t < num_threads_mediana; t++)
    {
        printf("\nCriando Thread mediana[%i]", t);
        pthread_create(&threads_mediana[t], NULL, calcula_mediana_linha, &dados_mediana[t]);
    }


     for (int p=0; p < num_threads_media; p++){
        pthread_join (threads_media[p], NULL);
     }
     for (int p=0; p < num_threads_mediana; p++){
        pthread_join (threads_mediana[p], NULL);
     }
    
    
    char nome_arquivo_resposta[60] = "resposta ";
    strcat(nome_arquivo_resposta, arquivo);
    escreve_resultado_arquivo(nome_arquivo_resposta, resultado_media, resultado_mediana, dimensoes[1], dimensoes[0]);
    return 0;
}
