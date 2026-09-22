#include <stdio.h>
#include <stdlib.h>
#include "floodfill.h"
#include "workers.h"
#include <pthread.h>

int main() {
    
    //versaoSequencial();
    int numThreads = 4;

    Worker workers[numThreads];

    // depois você define inicio/final de cada worker

    dividirTrabalho(workers, numThreads);

    return 0;
}



void versaoSequencial(){

    //mudar para o nome do arquivo que deseja ler
    Matriz matriz = lerMatriz("matriz 5x5.txt");

    int objetos = contador(&matriz);
    printf("Numero de objetos: %d\n", objetos);
}