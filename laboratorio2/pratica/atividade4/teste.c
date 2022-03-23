#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Separa o comando em tokens e retorna a qtde de tokens
int separa_tokens(char* string, char** store, char* delimitador){
    // Contador de tokens
    int count_tokens = 0;

    //copiando a string
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

int main()
{
    char* teste = "oi tudo bem com voce";

    char** tokens = calloc(20,  sizeof(char**)); 
    int qtde = separa_tokens(teste, tokens, " ");

    // printf("\nQtde: %i", qtde);

    for (int i = 0; i < qtde; i++)
    {
        printf("%s-", tokens[i]);
    }
    


    // printf("Antes do exec \n");
    // char* cmd[3] = {"ls", "-l", '\0'};
    // execv("/bin/ls", cmd);
    // printf("Depois do exec\n");
    // return 0;
}
