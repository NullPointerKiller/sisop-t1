#ifndef WORKERS_H
#define WORKERS_H

#include "matriz.h"

//dados que cada worker (thread) recebe
typedef struct {
    int id;
    Matriz *matriz;
    int **rotulos;     //matriz de rotulos compartilhada entre os workers
    int linhaInicio;   //primeira linha da faixa (inclusiva)
    int linhaFim;      //ultima linha da faixa (exclusiva)
    int contLocal;     //objetos encontrados dentro da faixa
} Worker;

//versao paralela (nao altera a matriz)
int contadorParalelo(Matriz *matriz, int numWorkers);

#endif
