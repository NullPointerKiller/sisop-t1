#include <stdio.h>
#include <stdlib.h>
#include "matriz.h"

Matriz lerMatriz(const char *nomeArquivo){

    Matriz matriz;

    FILE *arquivo = fopen(nomeArquivo, "r");

    if(arquivo==NULL){
        printf("erro pra abrir arquivo.\n");

        matriz.colunas = 0;
        matriz.linhas = 0;
        matriz.dados = NULL;
        
        return matriz;
    }

    if(fscanf(arquivo, "%dx%d", &matriz.linhas, &matriz.colunas) != 2 || matriz.linhas <= 0 || matriz.colunas <= 0){
        printf("erro: cabecalho invalido (esperado LINHASxCOLUNAS).\n");

        fclose(arquivo);
        matriz.colunas = 0;
        matriz.linhas = 0;
        matriz.dados = NULL;

        return matriz;
    }

    matriz.dados = (int **)malloc(matriz.linhas * sizeof(int *));

    for (int i = 0; i < matriz.linhas; i++) {
        matriz.dados[i] = malloc(matriz.colunas * sizeof(int));
    }

    for(int i = 0; i < matriz.linhas; i++){
        for(int j = 0; j < matriz.colunas; j++){
            if(fscanf(arquivo, "%d", &matriz.dados[i][j]) != 1){
                matriz.dados[i][j] = 0;
            }
        }
    }

    fclose(arquivo);

    return matriz;

}

void liberarMatriz(Matriz *matriz){
    for(int i = 0; i < matriz->linhas; i++){
        free(matriz->dados[i]);
    }
    free(matriz->dados);
}
