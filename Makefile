# Makefile do trabalho 1 de Sistemas Operacionais
# uso:
#   make                -> compila
#   make run            -> compila e roda com a matriz padrao e 4 workers
#   make run ARQ="x.txt" WORKERS=8
#   make test           -> roda as matrizes de tests/ com 1, 2, 3, 4 e 8 threads e confere o resultado
#   make bench          -> mede sequencial x paralelo e grava results/medicoes.csv
#   make clean          -> apaga os arquivos gerados

# mesmas flags do comando de referencia do enunciado (ANSI C89)
CC = cc
CFLAGS = -std=c89 -Wall -Wextra -pedantic -O2
LDFLAGS = -pthread

EXEC = contador
OBJS = main.o matriz.o floodfill.o workers.o

ARQ = matriz 500x500.txt
WORKERS = 4

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

main.o: main.c matriz.h floodfill.h workers.h
	$(CC) $(CFLAGS) -c main.c

matriz.o: matriz.c matriz.h
	$(CC) $(CFLAGS) -c matriz.c

floodfill.o: floodfill.c floodfill.h matriz.h
	$(CC) $(CFLAGS) -c floodfill.c

workers.o: workers.c workers.h floodfill.h matriz.h
	$(CC) $(CFLAGS) -pthread -c workers.c

run: $(EXEC)
	./$(EXEC) "$(ARQ)" $(WORKERS)

test: $(EXEC)
	sh tests/rodar_testes.sh ./$(EXEC)

bench: $(EXEC)
	sh tests/medir.sh ./$(EXEC)

clean:
	rm -f $(OBJS) $(EXEC) $(EXEC).exe

.PHONY: all run test bench clean
