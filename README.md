# sisop-t1
Trabalho 1 de Sistemas operacionais sobre contagem paralela de objetos em uma matriz

O programa conta os objetos de uma matriz binária (0 = fundo, 1 = primeiro plano), onde um objeto é um conjunto de células `1` ligadas por aresta ou por canto (conectividade 8). Tem duas versões que dão o mesmo resultado:

- **sequencial**: flood fill iterativo (pilha explícita) com uma matriz de visitados;
- **paralela (Pthreads)**: a matriz é dividida em faixas de linhas, uma por thread; cada thread rotula os objetos da sua faixa e depois a thread principal junta, com union-find, os objetos que atravessam as fronteiras entre faixas.

## Autoria

| Integrante | Matrícula |
|---|---|
| [PREENCHER nome completo] | [PREENCHER] |
| [PREENCHER nome completo] | [PREENCHER] |

Sistemas Operacionais - 2026/II - PUCRS - Prof. Filipo Mór

## Como compilar e rodar

Precisa de Linux ou macOS com `cc` (gcc ou clang) e `make`. O código é ANSI C (C89) e compila com as flags do enunciado: `-std=c89 -Wall -Wextra -pedantic -pthread`.

```
make
make run
make run ARQ="tests/obrigatorios/exemplo1_5x5.txt" WORKERS=4 (Definir arquivo e quantidade de threads)
make test
make bench
make clean
```

- `make run`: roda `./contador "matriz 500x500.txt" 4`.
- `make test`: roda as matrizes de `tests/` (as 5 obrigatórias do enunciado e os casos adicionais) com 1, 2, 3, 4 e 8 threads e confere se o paralelo e o sequencial dão o resultado esperado.
- `make bench`: mede as duas versões (5 repetições por configuração) e grava `results/medicoes.csv`. A matriz grande `tests/grande_2000x2000.txt` é gerada na primeira vez (semente fixa, então é sempre a mesma).

Sem `make`:

```
cc -std=c89 -Wall -Wextra -pedantic -O2 -pthread -o contador main.c matriz.c floodfill.c workers.c
./contador tests/obrigatorios/exemplo3_8x8.txt 2
```

### Entrada e saída

```
./contador [arquivo] [threads]
```

O arquivo começa com `LINHASxCOLUNAS` e depois os valores separados por espaço:

```
5x5
1 1 0 0 0
1 1 0 0 0
0 0 0 1 0
0 0 0 1 0
1 0 0 0 0
```

O programa roda a versão paralela, depois a sequencial sobre os mesmos dados, e imprime a quantidade de objetos e o tempo de cada uma (só a contagem é medida, sem a leitura do arquivo). Se as duas versões derem resultados diferentes, o programa avisa e sai com código 2. O número de threads é limitado a `[1, linhas]`.

## Arquitetura

| Arquivo | O que faz |
|---|---|
| `main.c` | Lê os argumentos, roda a versão paralela e a sequencial e imprime objetos e tempo |
| `matriz.c` / `matriz.h` | Leitura do arquivo e liberação da matriz |
| `floodfill.c` / `floodfill.h` | `contador` + `floodfill` (sequencial) e `floodfillFaixa` (usado pelas threads) |
| `workers.c` / `workers.h` | `dividirTrabalho` (faixas, threads, consolidação com union-find) e `trabalhar` (rotina de cada thread) |
| `tests/` | Matrizes obrigatórias e adicionais, `rodar_testes.sh` e `medir.sh` |
| `results/` | `medicoes.csv`, gráficos e `graficos.py`, que gera os gráficos a partir do CSV |
| `slides/` | Slides da apresentação |
| `RELATORIO_TECNICO.md` | Relatório técnico completo |

Fluxo: `lerMatriz` > `dividirTrabalho(p)` (threads rotulam suas faixas > `pthread_join` > união nas fronteiras) > `versaoSequencial` (`contador`) > imprime resultados > `liberarMatriz`.

- **Sem condição de corrida**: a matriz de entrada só é lida; cada thread escreve apenas nas linhas da sua faixa da matriz de rótulos e no seu `Worker`.
- **Sem mutex e sem deadlock**: a única sincronização é o `pthread_join`; a consolidação só começa depois que todas as threads terminaram.
- **Rótulo de cada objeto**: posição da célula semente + 1 (`linha * colunas + coluna + 1`), único na matriz inteira.
- **Total**: soma das contagens das faixas menos o número de uniões efetivas nas fronteiras.

Mais detalhes, testes e análise de desempenho estão no [RELATORIO_TECNICO.md](RELATORIO_TECNICO.md).

## Gerador de matrizes
O arquivo genMatriz.py gera matrizes de acordo com os parâmetros de tamanho passados. Utilizado para facilitar os testes com tamanhos diversificados.

```
python3 genMatriz.py [linhas] [colunas] [arquivo] [semente]
```

## Ferramentas e recursos externos

- POSIX Threads (biblioteca do sistema).
- Python 3 (`genMatriz.py`, `results/graficos.py`) e matplotlib (gráficos).
- Claude Code (Anthropic): usado na revisão do código, adaptação para C89, flood fill iterativo, scripts de teste e medição, gráficos e revisão da documentação. Todos os resultados foram conferidos rodando o programa.
