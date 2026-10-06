/* necessario para o clock_gettime aparecer no time.h compilando com -std=c89 */
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "floodfill.h"
#include "workers.h"
#include <pthread.h>

int versaoSequencial(Matriz *matriz);

/* tempo atual em segundos, para medir sequencial x paralelo */
double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {

    /* mudar para o nome do arquivo que deseja ler
       (ou passar por argumento: ./contador "arquivo.txt" numThreads) */
    const char *nomeArquivo = "matriz 500x500.txt";
    int numThreads = 4;
    Matriz matriz;
    Worker *workers;
    double inicio, tempoParalelo;
    int objetos, objetosSequencial;

    if(argc > 1){
        nomeArquivo = argv[1];
    }
    if(argc > 2){
        numThreads = atoi(argv[2]);
    }

    matriz = lerMatriz(nomeArquivo);
    if(matriz.dados == NULL){
        return 1;
    }

    /* nao tem como ter mais workers do que linhas */
    if(numThreads < 1){
        numThreads = 1;
    }
    if(numThreads > matriz.linhas){
        numThreads = matriz.linhas;
    }

    workers = (Worker *)malloc(numThreads * sizeof(Worker));
    if(workers == NULL){
        printf("erro: memoria insuficiente.\n");
        liberarMatriz(&matriz);
        return 1;
    }

    /* so a contagem e medida (a leitura do arquivo fica de fora) */
    inicio = agora();
    objetos = dividirTrabalho(&matriz, workers, numThreads);
    tempoParalelo = agora() - inicio;
    free(workers);

    if(objetos < 0){
        printf("erro na versao paralela.\n");
        liberarMatriz(&matriz);
        return 1;
    }

    printf("Numero de objetos (paralelo, %d threads): %d (%.6f s)\n", numThreads, objetos, tempoParalelo);

    /* a versao paralela nao altera a matriz, entao a sequencial roda sobre os mesmos dados */
    objetosSequencial = versaoSequencial(&matriz);

    liberarMatriz(&matriz);

    if(objetosSequencial < 0){
        return 1;
    }
    if(objetosSequencial != objetos){
        printf("ATENCAO: paralelo e sequencial deram resultados diferentes!\n");
        return 2;
    }

    return 0;
}



int versaoSequencial(Matriz *matriz){

    double inicio = agora();
    int objetos = contador(matriz);
    double tempoSequencial = agora() - inicio;

    if(objetos < 0){
        return -1;
    }

    printf("Numero de objetos (sequencial): %d (%.6f s)\n", objetos, tempoSequencial);

    return objetos;
}
