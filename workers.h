#ifndef WORKERS_H
#define WORKERS_H
#include "floodfill.h"
#include <pthread.h>

typedef struct {
    Matriz *matriz;
    int inicio;     //primeira linha da faixa (inclusiva)
    int final;      //ultima linha da faixa (exclusiva)
    int visitado;   //quantidade de objetos encontrados na faixa
} Worker;

//divide as linhas entre os workers, roda as threads e retorna o total de objetos
int dividirTrabalho(Matriz *matriz, Worker *workers, int numThreads);

void *trabalhar(void *arg);

#endif