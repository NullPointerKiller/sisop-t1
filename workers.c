#include <stdio.h>
#include <pthread.h>
#include "workers.h"

void dividirTrabalho(Worker *workers, int numThreads) {

    pthread_t threads[numThreads];

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, trabalhar, &workers[i]);
    }

    for(int i = 0; i<numThreads; i++){
        pthread_join(threads[i], NULL);
    }
}

void *trabalhar(void *arg) {

    Worker *worker = (Worker *)arg;

    printf("worker trabalhando de %d a %d\n", worker->inicio, worker->final);

    for(int i = worker->inicio; i < worker->final; i++) {

        for(int j = 0; j < worker->matriz->colunas; j++) {

            if(worker->matriz->dados[i][j] == 1) {
                floodFill(worker->matriz, i, j);
                worker->visitado++;
            }
        }
    }

    return NULL;
}

