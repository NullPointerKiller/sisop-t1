#ifndef MATRIZ_H
#define MATRIZ_H

typedef struct {
    int linhas;
    int colunas;
    int **dados;
} Matriz;

Matriz lerMatriz(const char *nomeArquivo);
void liberarMatriz(Matriz *matriz); 

#endif