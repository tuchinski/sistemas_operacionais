 - Para compilar, use o comando "make"
 - Para executar, use o comando "./atividade1"
    - O programa não tem saída. Para verificar a arvore de processos, o programa informa qual o comando que deve ser utilizado
    - Só é preciso copiar o comando apresentado, e executar em outro terminal
    - O programa tem um tempo de 10 segundos, que fica parado, para que seja possível visualizar a árvore dos processos

    - Para alterar o tempo de espera do programa, é necessário alterar o "define" TEMPO_ESPERA na linha 6 do código
    - Para alterar o tamanho da árvore criada, é necessário alterar o "define" NIVEL_ARVORE na linha 5 do código
    - OBS: quando qualquer parâmetro for alterado, o comando "make" deve ser executado novamente, para recompilar o código
    
    - Exemplo de uso:
        -> ./atividade1
        -> Para verificar a árvore de processos, use o comando "pstree -a 25503" 