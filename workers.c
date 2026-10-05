#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "workers.h"

//union-find usado para juntar objetos que cruzam a fronteira entre faixas
static int encontrar(int *pai, int x) {
    while(pai[x] != x){
        pai[x] = pai[pai[x]];
        x = pai[x];
    }
    return x;
}

//retorna 1 se juntou dois objetos diferentes, 0 se ja eram o mesmo
static int unir(int *pai, int a, int b) {
    a = encontrar(pai, a);
    b = encontrar(pai, b);
    if(a == b){
        return 0;
    }
    pai[b] = a;
    return 1;
}

int dividirTrabalho(Matriz *matriz, Worker *workers, int numThreads) {

    pthread_t threads[numThreads];

    //divide as linhas em faixas, as primeiras recebem uma linha a mais se sobrar
    int base = matriz->linhas / numThreads;
    int resto = matriz->linhas % numThreads;
    int linha = 0;

    for (int i = 0; i < numThreads; i++) {
        workers[i].matriz = matriz;
        workers[i].inicio = linha;
        linha += base + (i < resto ? 1 : 0);
        workers[i].final = linha;
        workers[i].visitado = 0;

        if(pthread_create(&threads[i], NULL, trabalhar, &workers[i]) != 0){
            printf("erro pra criar thread %d.\n", i);
            exit(1);
        }
    }

    //espera todos terminarem e soma o que cada um achou
    int total = 0;
    for(int i = 0; i<numThreads; i++){
        pthread_join(threads[i], NULL);
        total += workers[i].visitado;
    }

    //juncao: objeto que cruza a fronteira entre duas faixas foi contado mais de uma vez,
    //entao para cada par de vizinhos na fronteira com rotulos diferentes, junta e desconta
    int tamanho = matriz->linhas * matriz->colunas + 2;
    int *pai = malloc(tamanho * sizeof(int));
    for(int i = 0; i < tamanho; i++){
        pai[i] = i;
    }

    for(int i = 0; i < numThreads - 1; i++){
        int r = workers[i].final - 1; //ultima linha da faixa i (a proxima e a primeira da faixa i+1)
        for(int j = 0; j < matriz->colunas; j++){
            if(matriz->dados[r][j] == 0){
                continue;
            }
            //vizinhos de baixo: diagonal esquerda, baixo e diagonal direita
            for(int dj = -1; dj <= 1; dj++){
                int c = j + dj;
                if(c >= 0 && c < matriz->colunas && matriz->dados[r + 1][c] != 0){
                    if(unir(pai, matriz->dados[r][j], matriz->dados[r + 1][c])){
                        total--;
                    }
                }
            }
        }
    }

    free(pai);
    return total;
}

void *trabalhar(void *arg) {

    Worker *worker = (Worker *)arg;

    // printf("worker trabalhando de %d a %d\n", worker->inicio, worker->final);

    for(int i = worker->inicio; i < worker->final; i++) {

        for(int j = 0; j < worker->matriz->colunas; j++) {

            if(worker->matriz->dados[i][j] == 1) {
                worker->visitado++;
                //rotulo unico entre todos os workers: cada faixa usa um intervalo proprio (sempre >= 2)
                int rotulo = worker->inicio * worker->matriz->colunas + worker->visitado + 1;
                floodfillFaixa(worker->matriz, i, j, worker->inicio, worker->final, rotulo);
            }
        }
    }

    return NULL;
}
