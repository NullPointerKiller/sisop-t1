#include <stdio.h>
#include <stdlib.h>
#include "matriz.h"

Matriz lerMatriz(const char *nomeArquivo){
     printf("entrou no ler matriz\n");

    Matriz matriz;

    FILE *arquivo = fopen(nomeArquivo, "r");

    if(arquivo==NULL){
        printf("erro pra abrir arquivo.\n");

        matriz.colunas = 0;
        matriz.linhas = 0;
        matriz.dados = NULL;
        
        return matriz;
    }

    fscanf(arquivo, "%dx%d", &matriz.linhas, &matriz.colunas);

    
    matriz.dados = (int **)malloc(matriz.linhas * sizeof(int *));

    for (int i = 0; i < matriz.linhas; i++) {
        matriz.dados[i] = malloc(matriz.colunas * sizeof(int));
    }

    for(int i = 0; i < matriz.linhas; i++){
        for(int j = 0; j < matriz.colunas; j++){
            fscanf(arquivo, "%d", &matriz.dados[i][j]);
        }
    }

    fclose(arquivo);

    return matriz;

}

void liberarMatriz(Matriz *matriz){
     printf("entrou no liberar matriz\n");
    for(int i = 0; i < matriz->linhas; i++){
        free(matriz->dados[i]);
    }
    free(matriz->dados);
}