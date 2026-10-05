#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matriz.h"
#include "floodfill.h"
#include "workers.h"

// #define row 5
// #define col 5

//todo: fazer ler matriz de arquivo (feito em matriz.c)
// int matriz[row][col] = {
//     {1, 1, 0, 0, 1},
//     {0, 0, 1, 0, 0},
//     {0, 0, 0, 0, 1},
//     {0, 1, 0, 0, 1},
//     {1, 1, 0, 1, 1}
// };

//floodfill e contador foram movidos para floodfill.c (versao sequencial)
//e a versao paralela esta em workers.c

//tempo atual em segundos, para medir sequencial x paralelo
double agora() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {

    //mudar para o nome do arquivo que deseja ler
    //(ou passar por argumento: ./contador "arquivo.txt" numWorkers)
    const char *nomeArquivo = "matriz 5x5.txt";
    int numWorkers = 4;

    if(argc > 1){
        nomeArquivo = argv[1];
    }
    if(argc > 2){
        numWorkers = atoi(argv[2]);
    }

    Matriz matriz = lerMatriz(nomeArquivo);
    if(matriz.dados == NULL){
        return 1;
    }

    //nao tem como ter mais workers do que linhas (mesma regra do contadorParalelo)
    if(numWorkers < 1){
        numWorkers = 1;
    }
    if(numWorkers > matriz.linhas){
        numWorkers = matriz.linhas;
    }

    //paralelo primeiro, pois ele nao altera a matriz
    double inicio = agora();
    int objetosParalelo = contadorParalelo(&matriz, numWorkers);
    double tempoParalelo = agora() - inicio;

    //sequencial depois, pois o floodfill zera a matriz
    inicio = agora();
    int objetos = contador(&matriz);
    double tempoSequencial = agora() - inicio;

    printf("Numero de objetos (sequencial): %d (%.6f s)\n", objetos, tempoSequencial);
    printf("Numero de objetos (paralelo, %d workers): %d (%.6f s)\n", numWorkers, objetosParalelo, tempoParalelo);

    liberarMatriz(&matriz);
    return 0;
}
