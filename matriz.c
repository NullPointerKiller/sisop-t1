#include <stdio.h>
#include <stdlib.h>
#include "matriz.h"

static Matriz matrizVazia(void) {
    Matriz matriz;
    matriz.colunas = 0;
    matriz.linhas = 0;
    matriz.dados = NULL;
    return matriz;
}

Matriz lerMatriz(const char *nomeArquivo){

    Matriz matriz;
    FILE *arquivo;
    int i, j;

    arquivo = fopen(nomeArquivo, "r");

    if(arquivo==NULL){
        printf("erro pra abrir arquivo.\n");
        return matrizVazia();
    }

    /* primeira linha do arquivo: LINHASxCOLUNAS */
    if(fscanf(arquivo, "%dx%d", &matriz.linhas, &matriz.colunas) != 2 || matriz.linhas <= 0 || matriz.colunas <= 0){
        printf("erro: cabecalho invalido (esperado LINHASxCOLUNAS).\n");
        fclose(arquivo);
        return matrizVazia();
    }

    matriz.dados = (int **)malloc(matriz.linhas * sizeof(int *));
    if(matriz.dados == NULL){
        printf("erro: memoria insuficiente para a matriz.\n");
        fclose(arquivo);
        return matrizVazia();
    }

    for (i = 0; i < matriz.linhas; i++) {
        matriz.dados[i] = (int *)malloc(matriz.colunas * sizeof(int));
        if(matriz.dados[i] == NULL){
            printf("erro: memoria insuficiente para a matriz.\n");
            matriz.linhas = i; /* libera so as linhas ja alocadas */
            liberarMatriz(&matriz);
            fclose(arquivo);
            return matrizVazia();
        }
    }

    for(i = 0; i < matriz.linhas; i++){
        for(j = 0; j < matriz.colunas; j++){
            /* se faltar valor no arquivo, considera 0 */
            if(fscanf(arquivo, "%d", &matriz.dados[i][j]) != 1){
                matriz.dados[i][j] = 0;
            }
        }
    }

    fclose(arquivo);

    return matriz;

}

void liberarMatriz(Matriz *matriz){
    int i;
    if(matriz->dados == NULL){
        return;
    }
    for(i = 0; i < matriz->linhas; i++){
        free(matriz->dados[i]);
    }
    free(matriz->dados);
    matriz->dados = NULL;
}
