#ifndef WORKERS_H
#define WORKERS_H
#include "floodfill.h"
#include <pthread.h>

typedef struct {
    Matriz *matriz;
    int **rotulos;  /* matriz de rotulos compartilhada; cada worker so escreve nas suas linhas */
    int inicio;     /* primeira linha da faixa (inclusiva) */
    int final;      /* ultima linha da faixa (exclusiva) */
    int visitado;   /* quantidade de objetos encontrados na faixa */
    int erro;       /* 1 se a thread nao conseguiu alocar a sua pilha */
} Worker;

/* retorna o total de objetos, ou -1 em caso de erro */
int dividirTrabalho(Matriz *matriz, Worker *workers, int numThreads);

void *trabalhar(void *arg);

#endif
