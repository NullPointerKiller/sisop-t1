#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "workers.h"

//mesmo floodfill do sequencial, mas so anda dentro da faixa do worker
//e em vez de zerar a matriz marca o rotulo do objeto na matriz de rotulos
//(cada worker so escreve nas linhas da sua faixa, entao nao precisa de mutex)
static void floodfillFaixa(Worker *w, int linha, int coluna, int rotulo) {

    if (linha < w->linhaInicio || coluna < 0 || linha >= w->linhaFim || coluna >= w->matriz->colunas) {
        return;
    }

    //se nao fizer parte do objeto ou ja tiver sido visitado, volta
    if(w->matriz->dados[linha][coluna] == 0 || w->rotulos[linha][coluna] != 0){
        return;
    }

    //marca visitado com o rotulo do objeto
    w->rotulos[linha][coluna] = rotulo;

    // Chama recursivo os vizinhos
    floodfillFaixa(w, linha, coluna - 1, rotulo); // Esquerda
    floodfillFaixa(w, linha + 1, coluna - 1, rotulo); // Diagonal esquerda baixo
    floodfillFaixa(w, linha + 1, coluna, rotulo); // Baixo
    floodfillFaixa(w, linha + 1, coluna + 1, rotulo); // Diagonal direita baixo
    floodfillFaixa(w, linha, coluna + 1, rotulo); // Direita
    floodfillFaixa(w, linha - 1, coluna + 1, rotulo); // Diagonal direita cima
    floodfillFaixa(w, linha - 1, coluna, rotulo); // Cima
    floodfillFaixa(w, linha - 1, coluna - 1, rotulo); // Diagonal esquerda cima
}

//funcao executada por cada thread: conta os objetos da sua faixa de linhas
static void *rotinaWorker(void *arg) {
    Worker *w = (Worker *)arg;
    w->contLocal = 0;

    for(int i = w->linhaInicio; i < w->linhaFim; i++){
        for(int j = 0; j < w->matriz->colunas; j++){
            if(w->matriz->dados[i][j] == 1 && w->rotulos[i][j] == 0){
                w->contLocal++;
                //rotulo unico entre todos os workers: cada faixa usa um intervalo proprio
                int rotulo = w->linhaInicio * w->matriz->colunas + w->contLocal;
                floodfillFaixa(w, i, j, rotulo);
            }
        }
    }

    // printf("worker %d contou %d objetos (linhas %d a %d)\n", w->id, w->contLocal, w->linhaInicio, w->linhaFim - 1);
    return NULL;
}

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

int contadorParalelo(Matriz *matriz, int numWorkers) {
    // printf("entrou no contador paralelo\n");

    if(matriz->linhas <= 0 || matriz->colunas <= 0){
        return 0;
    }

    //nao tem como ter mais workers do que linhas
    if(numWorkers < 1){
        numWorkers = 1;
    }
    if(numWorkers > matriz->linhas){
        numWorkers = matriz->linhas;
    }

    //matriz de rotulos zerada (0 = ainda nao visitado)
    int **rotulos = malloc(matriz->linhas * sizeof(int *));
    for(int i = 0; i < matriz->linhas; i++){
        rotulos[i] = calloc(matriz->colunas, sizeof(int));
    }

    pthread_t *threads = malloc(numWorkers * sizeof(pthread_t));
    Worker *workers = malloc(numWorkers * sizeof(Worker));

    //divide as linhas em faixas, as primeiras recebem uma linha a mais se sobrar
    int base = matriz->linhas / numWorkers;
    int resto = matriz->linhas % numWorkers;
    int linha = 0;

    for(int k = 0; k < numWorkers; k++){
        workers[k].id = k;
        workers[k].matriz = matriz;
        workers[k].rotulos = rotulos;
        workers[k].linhaInicio = linha;
        linha += base + (k < resto ? 1 : 0);
        workers[k].linhaFim = linha;
        workers[k].contLocal = 0;

        if(pthread_create(&threads[k], NULL, rotinaWorker, &workers[k]) != 0){
            printf("erro pra criar thread %d.\n", k);
            exit(1);
        }
    }

    //espera todos terminarem e soma as contagens locais
    int total = 0;
    for(int k = 0; k < numWorkers; k++){
        pthread_join(threads[k], NULL);
        total += workers[k].contLocal;
    }

    //juncao: objeto que cruza a fronteira entre duas faixas foi contado mais de uma vez,
    //entao para cada par de vizinhos na fronteira com rotulos diferentes, junta e desconta
    int tamanho = matriz->linhas * matriz->colunas + 1;
    int *pai = malloc(tamanho * sizeof(int));
    for(int i = 0; i < tamanho; i++){
        pai[i] = i;
    }

    for(int k = 0; k < numWorkers - 1; k++){
        int r = workers[k].linhaFim - 1; //ultima linha da faixa k (a proxima e a primeira da faixa k+1)
        for(int j = 0; j < matriz->colunas; j++){
            if(rotulos[r][j] == 0){
                continue;
            }
            //vizinhos de baixo: diagonal esquerda, baixo e diagonal direita
            for(int dj = -1; dj <= 1; dj++){
                int c = j + dj;
                if(c >= 0 && c < matriz->colunas && rotulos[r + 1][c] != 0){
                    if(unir(pai, rotulos[r][j], rotulos[r + 1][c])){
                        total--;
                    }
                }
            }
        }
    }

    free(pai);
    free(workers);
    free(threads);
    for(int i = 0; i < matriz->linhas; i++){
        free(rotulos[i]);
    }
    free(rotulos);

    return total;
}
