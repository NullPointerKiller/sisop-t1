#include <stdio.h>
#include <stdlib.h>
#include "floodfill.h"

void floodfill(Matriz *matriz, int linha, int coluna) {
    printf("entrou no floodfill\n");
    
    if (linha < 0 || coluna < 0 || linha >= matriz->linhas || coluna >= matriz->colunas) {
        return;
    }
    
    //se nao fizer parte do objeto, volta
    if(matriz->dados[linha][coluna] == 0){
        return;
    }
    
    //marca visitado = ignora
    matriz->dados[linha][coluna] = 0;
    
    // Chama recursivo os vizinhos
    floodfill(matriz, linha, coluna - 1); // Esquerda
    floodfill(matriz, linha + 1, coluna - 1); // Diagonal esquerda baixo
    floodfill(matriz, linha + 1, coluna); // Baixo
    floodfill(matriz, linha + 1, coluna + 1); // Diagonal direita baixo
    floodfill(matriz, linha, coluna + 1); // Direita
    floodfill(matriz, linha - 1, coluna + 1); // Diagonal direita cima
    floodfill(matriz, linha - 1, coluna); // Cima
    floodfill(matriz, linha - 1, coluna - 1); // Diagonal esquerda cima
}

int contador(Matriz *matriz) {
    printf("entrou no contador\n");
    int cont = 0;

    for(int i = 0; i < matriz->linhas; i++){
        for(int j = 0; j < matriz->colunas; j++){
            if(matriz->dados[i][j] == 1){
                cont++;
                floodfill(matriz, i, j);
            }
        }
    }
    return cont;
}