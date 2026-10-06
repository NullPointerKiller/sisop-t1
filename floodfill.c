#include <stdio.h>
#include <stdlib.h>
#include "floodfill.h"

/* deslocamento dos 8 vizinhos (conectividade 8), na mesma ordem de antes:
   Esquerda, Diagonal esquerda baixo, Baixo, Diagonal direita baixo,
   Direita, Diagonal direita cima, Cima, Diagonal esquerda cima */
static const int dLinha[8]  = { 0,  1, 1, 1, 0, -1, -1, -1 };
static const int dColuna[8] = {-1, -1, 0, 1, 1,  1,  0, -1 };

/* flood fill iterativo: usa uma pilha explicita em vez de recursao, pra nao estourar
   a pilha de chamadas em objetos grandes. A celula e marcada como visitada ao empilhar,
   entao cada celula entra na pilha uma vez so e a pilha nunca passa de linhas*colunas */
void floodfill(Matriz *matriz, char **visitado, int *pilha, int linha, int coluna) {
    int topo = 0;
    int pos, l, c, k;

    visitado[linha][coluna] = 1;
    pilha[topo++] = linha * matriz->colunas + coluna;

    while(topo > 0){
        pos = pilha[--topo];

        for(k = 0; k < 8; k++){
            l = pos / matriz->colunas + dLinha[k];
            c = pos % matriz->colunas + dColuna[k];

            if (l < 0 || c < 0 || l >= matriz->linhas || c >= matriz->colunas) {
                continue;
            }

            /* se nao fizer parte do objeto ou ja tiver sido visitado, ignora */
            if(matriz->dados[l][c] != 1 || visitado[l][c]){
                continue;
            }

            visitado[l][c] = 1;
            pilha[topo++] = l * matriz->colunas + c;
        }
    }
}

static void liberarVisitado(char **visitado, int linhas) {
    int i;
    for(i = 0; i < linhas; i++){
        free(visitado[i]);
    }
    free(visitado);
}

/* versao sequencial: nao altera a matriz (usa a matriz visitado)
   retorna a quantidade de objetos, ou -1 se faltar memoria */
int contador(Matriz *matriz) {
    int cont = 0;
    int i, j;
    char **visitado;
    int *pilha;

    visitado = (char **)malloc(matriz->linhas * sizeof(char *));
    pilha = (int *)malloc((size_t)matriz->linhas * matriz->colunas * sizeof(int));
    if(visitado == NULL || pilha == NULL){
        printf("erro: memoria insuficiente na versao sequencial.\n");
        free(visitado);
        free(pilha);
        return -1;
    }

    for(i = 0; i < matriz->linhas; i++){
        visitado[i] = (char *)calloc(matriz->colunas, sizeof(char));
        if(visitado[i] == NULL){
            printf("erro: memoria insuficiente na versao sequencial.\n");
            liberarVisitado(visitado, i);
            free(pilha);
            return -1;
        }
    }

    for(i = 0; i < matriz->linhas; i++){
        for(j = 0; j < matriz->colunas; j++){
            if(matriz->dados[i][j] == 1 && !visitado[i][j]){
                cont++;
                floodfill(matriz, visitado, pilha, i, j);
            }
        }
    }

    liberarVisitado(visitado, matriz->linhas);
    free(pilha);
    return cont;
}

/* flood fill usado pelos workers: mesmo algoritmo, mas so anda dentro da faixa [inicio, final)
   e marca o rotulo do objeto na matriz de rotulos (0 = nao visitado). Como cada thread so escreve
   nas linhas da sua faixa, nao precisa de mutex. A pilha guarda a posicao relativa ao inicio da
   faixa, entao basta uma pilha do tamanho da faixa por thread */
void floodfillFaixa(Matriz *matriz, int **rotulos, int *pilha, int linha, int coluna, int inicio, int final, int rotulo) {
    int topo = 0;
    int pos, l, c, k;

    rotulos[linha][coluna] = rotulo;
    pilha[topo++] = (linha - inicio) * matriz->colunas + coluna;

    while(topo > 0){
        pos = pilha[--topo];

        for(k = 0; k < 8; k++){
            l = inicio + pos / matriz->colunas + dLinha[k];
            c = pos % matriz->colunas + dColuna[k];

            if (l < inicio || c < 0 || l >= final || c >= matriz->colunas) {
                continue;
            }

            if(matriz->dados[l][c] != 1 || rotulos[l][c] != 0){
                continue;
            }

            rotulos[l][c] = rotulo;
            pilha[topo++] = (l - inicio) * matriz->colunas + c;
        }
    }
}
