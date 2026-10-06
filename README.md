# sisop-t1
Trabalho 1 de Sistemas operacionais sobre contagem paralela de objetos em uma matriz

## Como compilar e rodar

```
make
make run
make run ARQ="matriz 5x5.txt" WORKERS=4 (Definir arquivo e quantidade de threads)
make clean
```

### Gerador de matrizes
O arquivo genMatriz.py gera matrizes de acordo com os parâmetros de tamanho passados. Utilizado para facilitar os testes com tamanhos diversificados.