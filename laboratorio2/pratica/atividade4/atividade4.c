#include <stdio.h>
#include <stdio_ext.h> //__fpurge
#include <string.h> // strtok
#include <stdlib.h> //calloc
#include <sys/wait.h> //wait
#include <unistd.h> //fork



/*
    Faça uma interface de shell simples que fornece um prompt ao usuário para executar comandos do
    shell do sistema. Se o comando for executado em segundo plano (&), a interface deve possibilitar a
    execução de outros comandos. Caso contrário, a interface deve esperar o retorno do comando e, em
    seguida, exibir o prompt novamente.
*/
// Pegar string com espaco do terminal em C: https://www.geeksforgeeks.org/taking-string-input-space-c-3-different-methods/

// Funcao que verifica o tamanho da string
int tam_string(char* str){
    int i = 0;
    char caractere_atual = str[i];
    while(caractere_atual != '\0'){
        i++;
        caractere_atual = str[i];
    }
    printf("Tamanho: %d", i);
}

// Funcao que cria um processo filho e carrega o comando solicitado no processo
// Local dos executaveis: /bin
void executa_comando_filho(char** tokens, int qtde_tokens){

    tokens[qtde_tokens] = NULL;
    pid_t pid_filho;
    pid_filho = fork();

    if(pid_filho){
        // Codigo executado pelo pai
        wait(NULL);
        printf("Codigo do pai\n");
    } else {
       // Codigo executado pelo filho
        char* nome_programa = tokens[0];
        printf("Executando o programa %s\n",nome_programa);

        for (int i = 0; i < qtde_tokens; i++)
        {
            printf("\n%s\n", tokens[i]);
        }
        
        execve("/bin/ls", tokens, NULL);
        exit(0);
    }
}

//Separa o comando em tokens e retorna a qtde de tokens
int separa_tokens(char* string, char** store, char* delimitador){
    // Contador de tokens
    int count_tokens = 0;

    //copiando a string
    //necessario copiar a string pra nao dar segmentation fault
    char copia_string[100];
    strcpy(copia_string, string);
    
    // criando variavel que vai armazenar o token atual
    char *token = strtok(copia_string, delimitador);

    // salvando o token no ponteiro passado por parametro
    while (token != NULL){
        store[count_tokens] = token;
        token = strtok(NULL, delimitador);
        count_tokens++;
    }

    // retornando a qtde de tokens
    return count_tokens;
}

int main(int argc, char const *argv[]){
    char comando_digitado[100];
    char* tokens_comando[50];

    while (1) {
        printf("\nDigite o comando desejado: ");
        __fpurge(stdin);
        scanf("%[^\n]s", comando_digitado);
        // __fpurge(stdin);

        char** tokens = calloc(40, sizeof(char**));
        int qtde_tokens = 0;
        qtde_tokens = separa_tokens(comando_digitado, tokens, " ");

        if(qtde_tokens == 0){
            printf("Digite algo");
        } else {
            executa_comando_filho(tokens, qtde_tokens);
        }         

       
    }
}
