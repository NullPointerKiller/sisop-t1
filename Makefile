# Makefile do trabalho 1 de Sistemas Operacionais
# uso:
#   make                -> compila
#   make run            -> compila e roda com a matriz padrao e 4 workers
#   make run ARQ="x.txt" WORKERS=8
#   make clean          -> apaga os arquivos gerados

CC = gcc
CFLAGS = -Wall -Wextra -O2
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

clean:
	rm -f $(OBJS) $(EXEC) $(EXEC).exe

.PHONY: all run clean
