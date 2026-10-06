#ifndef FLOODFILL_H
#define FLOODFILL_H
#include "matriz.h"

void floodfill(Matriz *matriz, char **visitado, int *pilha, int linha, int coluna);
int contador(Matriz *matriz);
void floodfillFaixa(Matriz *matriz, int **rotulos, int *pilha, int linha, int coluna, int inicio, int final, int rotulo);

#endif
