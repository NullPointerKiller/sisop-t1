#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "floodfill.h"
#include "workers.h"
#include <pthread.h>

int versaoSequencial(const char *nomeArquivo);

//tempo atual em segundos, para medir sequencial x paralelo
double agora() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {

    //mudar para o nome do arquivo que deseja ler
    //(ou passar por argumento: ./contador "arquivo.txt" numThreads)
    const char *nomeArquivo = "matriz 5x5.txt";
    int numThreads = 4;

    if(argc > 1){
        nomeArquivo = argv[1];
    }
    if(argc > 2){
        numThreads = atoi(argv[2]);
    }

    Matriz matriz = lerMatriz(nomeArquivo);
    if(matriz.dados == NULL){
        return 1;
    }

    //nao tem como ter mais workers do que linhas
    if(numThreads < 1){
        numThreads = 1;
    }
    if(numThreads > matriz.linhas){
        numThreads = matriz.linhas;
    }

    Worker workers[numThreads];

    // inicio/final de cada worker sao definidos dentro do dividirTrabalho
    double inicio = agora();
    int objetos = dividirTrabalho(&matriz, workers, numThreads);
    double tempoParalelo = agora() - inicio;

    printf("Numero de objetos (paralelo, %d threads): %d (%.6f s)\n", numThreads, objetos, tempoParalelo);

    liberarMatriz(&matriz);

    //roda a versao sequencial pra comparar (le o arquivo de novo, pois a matriz foi alterada)
    versaoSequencial(nomeArquivo);

    return 0;
}



int versaoSequencial(const char *nomeArquivo){

    //mudar para o nome do arquivo que deseja ler
    Matriz matriz = lerMatriz(nomeArquivo);
    if(matriz.dados == NULL){
        return 0;
    }

    double inicio = agora();
    int objetos = contador(&matriz);
    double tempoSequencial = agora() - inicio;

    printf("Numero de objetos (sequencial): %d (%.6f s)\n", objetos, tempoSequencial);

    liberarMatriz(&matriz);
    return objetos;
}
