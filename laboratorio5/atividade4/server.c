#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


int main(){
    int fd, conexao, pid_filho, pipe1[2], pipe2[2];
    int flag = 1;
    char message[100] = " ";
    char buffer1[4], buffer2[4]; 
    
	fd = socket(AF_INET, SOCK_STREAM, 0);
    
    
    memset(message, '\0',sizeof(message));
    memset(buffer1, '\0',sizeof(buffer1));
    memset(buffer2, '\0',sizeof(buffer2));

    //Configura o Socket.
    struct sockaddr_in serv; 
    memset(&serv, '\0', sizeof(serv)); 
	serv.sin_family = AF_INET;
	serv.sin_port = htons(1234); 
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1){
        printf("Erro ao iniciar o servidor!!");
        exit(0);
    }else{
        printf("Servidor Iniciado!\n");
    }
	 
	listen(fd, 2);

    //Abre os pipes.
    pipe(pipe1);
    pipe(pipe2);

    //Fica aguardando uma conexão ser aceita pelo socket.
	while((conexao = accept(fd, (struct sockaddr *)NULL, NULL))) {
        int flag_fork = flag;//Flag diferenciar os filhos.
        if(flag == 1){ 
            flag = 0; 
        }

		if ( (pid_filho = fork ()) == 0 ){
            if (flag_fork == 1){
                send(conexao, "Yes", 4, 0);

                while (!read(pipe1[0], buffer1, sizeof(buffer1))){
                    // fica esperando
                }
                send(conexao, "Con", 4, 0);

                while (!read(pipe1[0], buffer1, sizeof(buffer1))){
                    // fica esperando
                }
                send(conexao, buffer1, sizeof(buffer1), 1);

                //gerencia o pipe e o que precisa ser enviado via socket
                while (recv(conexao, message, 100, 0) > 0) {
                    close(pipe2[0]);
                    write(pipe2[1], message, 4);

                    close(pipe1[1]);
                    read(pipe1[0], buffer1, sizeof(buffer1));
                    send(conexao, buffer1, 4, 0);
                }
            }
            else {
                send(conexao, "No", 4, 0);//Emite uma mensagem avisando que é o segundo a se conectar.

                close(pipe1[0]);
                write(pipe1[1], "Sta", 4);

                //gerencia o pipe e o que precisa ser enviado via socket
                while (recv(conexao, message, 100, 0) > 0) {
                    close(pipe1[0]);
                    write(pipe1[1], message, 4);

                    close(pipe2[1]);
                    read(pipe2[0], buffer2, sizeof(buffer2));
                    send(conexao, buffer2, 4, 0);
                }
            }
        }
	}
    close(conexao);
	exit(0);
}