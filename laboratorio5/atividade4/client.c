#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    Faça um Jogo da Velha que possibilite dois clientes se conectarem ao servidor usando sockets. No
    servidor, para cada cliente será criado um processo filho. Os processos filhos devem se comunicar via
    pipe para trocarem os lances dos jogadores. Ao final do jogo, os processos filhos devem ser finalizados.

    Autores: Ilzimara Silva, Leonardo Tuchinski e Lucas Gabriel

    Data: 23/04/2022

*/

int contador;
char velha[3][3];

void inicio_velha()
{
    memset(&velha, ' ', sizeof(velha));
    contador = 0;
}

void desenha_velha()
{
    printf("-------------\n");
    printf("| %c | %c | %c |\n", velha[0][0], velha[0][1], velha[0][2]);
    printf("-------------\n");
    printf("| %c | %c | %c |\n", velha[1][0], velha[1][1], velha[1][2]);
    printf("-------------\n");
    printf("| %c | %c | %c |\n", velha[2][0], velha[2][1], velha[2][2]);
    printf("-------------\n");

}

void verifica_ganhador()
{
    char ganhador = ' ';
    for (int i = 0; i < 3; i++){
        if ((velha[i][0] == velha[i][1]) && (velha[i][1] == velha[i][2])){
            ganhador = velha[i][1];
            break;
        }
        if ((velha[0][i] == velha[1][i]) && (velha[1][i] == velha[2][i])){
            ganhador = velha[1][i];
            break;
        }
    }

    if ((velha[0][0] == velha[1][1]) && (velha[1][1] == velha[2][2])){
        ganhador = velha[1][1];
    }
    else if ((velha[0][2] == velha[1][1]) && (velha[1][1] == velha[2][0])){
        ganhador = velha[1][1];
    }

    if(ganhador == ' '){
        if(contador == 9){
            printf("\n EMPATE!!!\n");
            exit(0);
        }
        return;
    }
    else{
        printf("\nGanhador é o %c!!\n", ganhador);
        exit(0);
    }
}

int marca_velha(int l, int c, char sinal)
{
    if (velha[l][c] != ' ')
        return -1;
    velha[l][c] = sinal;
    return 1;
}

int main()
{
    int fd, first_conection = 1, line, col;
    char message[100], turn = 'X';

    fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configura o Socket.
    struct sockaddr_in serv;
    memset(&serv, '\0', sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(1234);

    // Certifica de estar limpa os espaços de memória.
    memset(message, '\0', sizeof(message));

    // Se conecta com o servidor.
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr);
    if(connect(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1){
        printf("Erro ao se conectar ao servidor!");
        exit(1);
    }
    printf("Aguardando outro cliente se conectar ao jogo\n");

    // Chama as funções para inicia e desenhar o tabuleiro.
    inicio_velha();
    desenha_velha();

    // Fica em loop enquanto não existir um ganhador.
    while (1)
    {
        // Faz o controle de qual jogador entrou primeiro.
        if (first_conection == 1)
        { // Verifica se é o primeiro ou segundo jogador.
            recv(fd, message, 4, 0);
            if ((strcmp(message, "Yes") == 0))
            { // Aguarda outro jogador se conectar.
                recv(fd, message, 4, 0);
                printf("Jogador encontrado, aguarde o movimento dele\n");
                if ((strcmp(message, "Con") == 0))
                { // Recebe a primeira jogada do oponente.
                    first_conection = -1;
                    recv(fd, message, 4, 0);
                    printf("\nPrimeira jogado do oponente: %s \n", message);

                    line = atoi(&message[0]);
                    col = atoi(&message[2]);
                    turn = 'O';
                    if (marca_velha(line, col, turn) != 1)
                    {
                        printf("Jogada do oponente foi invalida e ele perdeu a vez");
                    }
                    else
                    {
                        contador++;
                        // printf("\nJogada do oponente feita com sucesso.\n");
                        desenha_velha();
                    }

                    memset(message, '\0', sizeof(message));
                    fflush(stdout);
                }
            }
            else
            {
                first_conection = -1;
                printf("Já existe um oponente esperando\n");
            }
        }
        // Captura o movimento do player e faz a marcação no tabuleiro.
        printf("\nFaça uma jogada: ");
        fgets(message, 100, stdin);
        line = atoi(&message[0]);
        col = atoi(&message[2]);
        turn = 'X';
        if (marca_velha(line, col, turn) != 1)
        {
            printf("Jogada invalido, perdeu a vez");
        }
        else
        {
            printf("\nJogada feita com sucesso\n");
            contador++;
            desenha_velha();
        }
        send(fd, message, 4, 0); // Manda para o servidor a jogada realizada.

        // Limpa a variavel e o stdin.
        memset(message, '\0', sizeof(message));
        fflush(stdin);

        // Verifica se ja existe algum ganhador.
        verifica_ganhador();

        // Fica esperando o movimento do oponente.
        recv(fd, message, 4, 0);
        printf("\nJogada do oponente: %s", message);
        line = atoi(&message[0]);
        col = atoi(&message[2]);
        turn = 'O';
        if (marca_velha(line, col, turn) != 1)
        {
            printf("JJogada do oponente foi invalida e ele perdeu a vez");
        }
        else
        {
            printf("\nJogada do oponente feita com sucesso\n");
            contador++;
            desenha_velha();
        }

        // Limpa a variavel e o stdout.
        memset(message, '\0', sizeof(message));
        fflush(stdout);

        // Verifica se a jogada feita pelo oponente resulta em uma vitoria ou empate.
        verifica_ganhador();
        
    }
    close(fd);
    return 0;
}