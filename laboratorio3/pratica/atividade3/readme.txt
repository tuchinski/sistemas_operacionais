 - Para compilar, use o comando "make"
 - O código divide a qtde de threads para realizar as tarefas
   - Ou seja, se foram escolhidas 4 threads, 2 serão usadas para o cálculo da média e 2 para o cálculo da mediana
 - Para alterar a qtde de Threads, mudar o valor da constante NUM_THREADS, na linha 5 do código
 - Existem 2 formas de usar o programa, usando uma matriz existente, ou criando uma com dados aleatorios

 - Para executar usando uma matriz existente, use o comando "./atividade3 <nome_arquivo_matriz>"
    - Será executado automaticamente o cálculo da média e mediana
    - A resposta será escrita em um arquivo, com nome "resposta <nome_arquivo_matriz>"

 - Para executar criando uma matriz aleatoria, use o comando "./atividade3"
    - Sera solicitado se deseja mesmo criar uma nova matriz, pois nenhum nome de arquivo foi digitado
    - Digite qualquer caractere, e depois será solicitado quantas linhas e colunas devem ter a nova matriz
    - A matriz gerada fica salva no arquivo "matriz_gerada"
    - Logo após será realizada a média e mediana
    - A resposta será escrita em um arquivo, com nome "resposta matriz_gerada"
