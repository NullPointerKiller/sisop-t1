#ifndef FLOODFILL_H
#define FLOODFILL_H

#include "matriz.h"

//versao sequencial (altera a matriz, zerando os objetos visitados)
void floodfill(Matriz *matriz, int linha, int coluna);
int contador(Matriz *matriz);

#endif
