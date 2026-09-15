#include <stdio.h>
#include <stdlib.h>
// #include "matriz.h"
#include "floodfill.h"

int main() {

    //mudar para o nome do arquivo que deseja ler
    Matriz matriz = lerMatriz("matriz 5x5.txt");

    int objetos = contador(&matriz);
    printf("Numero de objetos: %d\n", objetos);
    return 0;
}