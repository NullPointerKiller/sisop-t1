#include <stdio.h>
#include <stdlib.h>
#include "floodfill.h"

void floodfill(Matriz *matriz, int linha, int coluna) {
    
    if (linha < 0 || coluna < 0 || linha >= matriz->linhas || coluna >= matriz->colunas) {
        return;
    }
    
    if(matriz->dados[linha][coluna] == 0){
        return;
    }
    
    matriz->dados[linha][coluna] = 0;
    
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

void floodfillFaixa(Matriz *matriz, int linha, int coluna, int inicio, int final, int rotulo) {

    if (linha < inicio || coluna < 0 || linha >= final || coluna >= matriz->colunas) {
        return;
    }

    if(matriz->dados[linha][coluna] != 1){
        return;
    }

    matriz->dados[linha][coluna] = rotulo;

    floodfillFaixa(matriz, linha, coluna - 1, inicio, final, rotulo); // Esquerda
    floodfillFaixa(matriz, linha + 1, coluna - 1, inicio, final, rotulo); // Diagonal esquerda baixo
    floodfillFaixa(matriz, linha + 1, coluna, inicio, final, rotulo); // Baixo
    floodfillFaixa(matriz, linha + 1, coluna + 1, inicio, final, rotulo); // Diagonal direita baixo
    floodfillFaixa(matriz, linha, coluna + 1, inicio, final, rotulo); // Direita
    floodfillFaixa(matriz, linha - 1, coluna + 1, inicio, final, rotulo); // Diagonal direita cima
    floodfillFaixa(matriz, linha - 1, coluna, inicio, final, rotulo); // Cima
    floodfillFaixa(matriz, linha - 1, coluna - 1, inicio, final, rotulo); // Diagonal esquerda cima
}
