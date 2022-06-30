# Fatshell
- implementação de um terminal virtual que possibilita a navegação em discos com formato FAT32
- para a execução do programa, é necessário ter instalado no computador o Python 3.x
- execução do código
``` shell
python3 fatshell.py <nome_da_imagem>
```
Segue as operações que podem ser realizadas no fatshell:
- info: exibe informações do disco e da FAT.
- cluster <num>: exibe o conteúdo do bloco num no formato texto.
- pwd: exibe o diretório corrente (caminho absoluto).
- attr <file | dir>: exibe os atributos de um arquivo (file) ou diretório (dir).
- cd <path>: altera o diretório corrente para o definido como path.
- touch <file>: cria o arquivo file com conteúdo vazio.
- mkdir <dir>: cria o diretório dir vazio.
- rm <file>: remove o arquivo file do sistema.
- rmdir <dir>: remove o diretório dir, se estiver vazio.
- cp <source_path> <target_path>: copia um arquivo de origem (source_path) para destino (target_path).
- mv <source_path> <target_path>: move um arquivo de origem (source_path) para destino (target_path).
- rename <file> <newfilename> : renomeia arquivo file para newfilename.
- ls: listar os arquivos e diretórios do diretório corrente.