#ifndef WORKERS_H
#define WORKERS_H
#include "floodfill.h"
#include <pthread.h>

typedef struct {
    Matriz *matriz;
    int inicio;
    int final;
    int visitado;
} Worker;

void dividirTrabalho(Worker *workers, int numThreads);

void *trabalhar(void *arg);

#endif