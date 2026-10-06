#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "workers.h"

/* union-find usado para juntar objetos que cruzam a fronteira entre faixas (path halving) */
static int encontrar(int *pai, int x) {
    while(pai[x] != x){
        pai[x] = pai[pai[x]];
        x = pai[x];
    }
    return x;
}

/* retorna 1 se juntou dois objetos diferentes, 0 se ja eram o mesmo */
static int unir(int *pai, int a, int b) {
    a = encontrar(pai, a);
    b = encontrar(pai, b);
    if(a == b){
        return 0;
    }
    pai[b] = a;
    return 1;
}

static void liberarRotulos(int **rotulos, int linhas) {
    int i;
    for(i = 0; i < linhas; i++){
        free(rotulos[i]);
    }
    free(rotulos);
}

int dividirTrabalho(Matriz *matriz, Worker *workers, int numThreads) {

    pthread_t *threads;
    int **rotulos;
    int *pai;
    int base, resto, linha, criadas, erro, total, tamanho;
    int i, j, dj, c, r;

    threads = (pthread_t *)malloc(numThreads * sizeof(pthread_t));
    rotulos = (int **)malloc(matriz->linhas * sizeof(int *));
    if(threads == NULL || rotulos == NULL){
        printf("erro: memoria insuficiente na versao paralela.\n");
        free(threads);
        free(rotulos);
        return -1;
    }

    /* matriz de rotulos zerada (0 = ainda nao visitado); a matriz de entrada so e lida */
    for(i = 0; i < matriz->linhas; i++){
        rotulos[i] = (int *)calloc(matriz->colunas, sizeof(int));
        if(rotulos[i] == NULL){
            printf("erro: memoria insuficiente na versao paralela.\n");
            liberarRotulos(rotulos, i);
            free(threads);
            return -1;
        }
    }

    /* divide as linhas em faixas, as primeiras recebem uma linha a mais se sobrar */
    base = matriz->linhas / numThreads;
    resto = matriz->linhas % numThreads;
    linha = 0;
    criadas = 0;
    erro = 0;

    for (i = 0; i < numThreads; i++) {
        workers[i].matriz = matriz;
        workers[i].rotulos = rotulos;
        workers[i].inicio = linha;
        linha += base + (i < resto ? 1 : 0);
        workers[i].final = linha;
        workers[i].visitado = 0;
        workers[i].erro = 0;

        if(pthread_create(&threads[i], NULL, trabalhar, &workers[i]) != 0){
            printf("erro pra criar thread %d.\n", i);
            erro = 1;
            break;
        }
        criadas++;
    }

    /* espera todas as threads criadas (mesmo se alguma falhou) e soma o que cada uma achou;
       o join funciona como barreira: a consolidacao so comeca depois que todas terminaram */
    total = 0;
    for(i = 0; i < criadas; i++){
        if(pthread_join(threads[i], NULL) != 0){
            printf("erro no pthread_join da thread %d.\n", i);
            erro = 1;
        }
        if(workers[i].erro){
            erro = 1;
        }
        total += workers[i].visitado;
    }
    free(threads);

    if(erro){
        liberarRotulos(rotulos, matriz->linhas);
        return -1;
    }

    /* juncao: objeto que cruza a fronteira entre duas faixas foi contado mais de uma vez,
       entao para cada par de vizinhos na fronteira com rotulos diferentes, junta e desconta */
    tamanho = matriz->linhas * matriz->colunas + 1;
    pai = (int *)malloc(tamanho * sizeof(int));
    if(pai == NULL){
        printf("erro: memoria insuficiente na consolidacao.\n");
        liberarRotulos(rotulos, matriz->linhas);
        return -1;
    }
    for(i = 0; i < tamanho; i++){
        pai[i] = i;
    }

    for(i = 0; i < numThreads - 1; i++){
        r = workers[i].final - 1; /* ultima linha da faixa i (a proxima e a primeira da faixa i+1) */
        for(j = 0; j < matriz->colunas; j++){
            if(rotulos[r][j] == 0){
                continue;
            }
            /* vizinhos de baixo: diagonal esquerda, baixo e diagonal direita */
            for(dj = -1; dj <= 1; dj++){
                c = j + dj;
                if(c >= 0 && c < matriz->colunas && rotulos[r + 1][c] != 0){
                    if(unir(pai, rotulos[r][j], rotulos[r + 1][c])){
                        total--;
                    }
                }
            }
        }
    }

    free(pai);
    liberarRotulos(rotulos, matriz->linhas);
    return total;
}

void *trabalhar(void *arg) {

    Worker *worker = (Worker *)arg;
    int *pilha;
    int i, j;

    printf("worker trabalhando de %d a %d\n", worker->inicio, worker->final);

    /* pilha propria da thread, do tamanho da faixa */
    pilha = (int *)malloc((size_t)(worker->final - worker->inicio) * worker->matriz->colunas * sizeof(int));
    if(pilha == NULL){
        worker->erro = 1;
        return NULL;
    }

    for(i = worker->inicio; i < worker->final; i++) {

        for(j = 0; j < worker->matriz->colunas; j++) {

            if(worker->matriz->dados[i][j] == 1 && worker->rotulos[i][j] == 0) {
                worker->visitado++;
                /* rotulo = posicao da celula semente + 1: unico na matriz toda e nunca 0 */
                floodfillFaixa(worker->matriz, worker->rotulos, pilha, i, j, worker->inicio, worker->final,
                               i * worker->matriz->colunas + j + 1);
            }
        }
    }

    free(pilha);
    return NULL;
}
