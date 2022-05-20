# Atividade que implementa o problema clássico do Barbeiro dorminhoco com monitores

- Informações:
    - Dentro do arquivo `barbeiro_monitor.h` existem algumas constantes:
        - `MAX_CLIENTES`: quantidade máxima de clientes que podem ficar esperando, caso algum cliente chege e a fila esteja cheia, ele vai embora
        `TEMPO_ATENDIMENTO_BARBEIRO`: tempo que o barbeiro leva para atender cada cliente (sleep que permite a melhor visualização do funcionamento do algoritmo)
        - `QTDE_CLIENTES_ATENDIMENTO`: define quantas threads de clientes serão criadas para atendimento 
        - `FREQUENCIA_CHEGADA_CLIENTES`: define a frequência(em segundos) da criação de clientes para atendimento (sleep que permite a melhor visualização do funcionamento do algoritmo)
    
    - Para que algum parâmetro seja alterado, é necessário somente alterar seus valores no arquivo `barbeiro_monitor.h`

- Execução:
    - Para executar o código, siga os passos abaixo no terminal

``` shell
make
./barbeiro
```