#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void countRegressivo(){
    int count = 9;
    printf("\nO programa será encerrado em... ");
    while (count >= 0){
        printf("\n%d", count);
        sleep(1);
        count--;
    }
}

void handlerAlarm(int sinal){
    raise(SIGKILL);
}

//função para tratar o sinal
void handlerInt(int sinal){
    alarm(10);
    countRegressivo(); 
}


int main(){
    // Mostra o PID obtido
    printf("PID: %d\n\n", getpid());

    //Realiza a associação com das funções de tratamento
    signal(SIGINT, handlerInt);
    signal(SIGALRM, handlerAlarm);

    printf("Aguardando o sinal...\n");
    while (1);

    return 0;
}