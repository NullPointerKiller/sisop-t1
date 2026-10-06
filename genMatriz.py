import random
import sys

# uso: python3 genMatriz.py [linhas] [colunas] [arquivo] [semente]
# sem argumentos gera a matriz 500x500 de antes
linhas = int(sys.argv[1]) if len(sys.argv) > 1 else 500
colunas = int(sys.argv[2]) if len(sys.argv) > 2 else 500
arquivo = sys.argv[3] if len(sys.argv) > 3 else f"matriz {linhas}x{colunas}.txt"

# semente fixa deixa a matriz reproduzivel (mesma matriz em qualquer maquina)
if len(sys.argv) > 4:
    random.seed(int(sys.argv[4]))

with open(arquivo, "w") as f:
    f.write(f"{linhas}x{colunas}\n")

    for i in range(linhas):
        linha = [str(random.randint(0, 1)) for _ in range(colunas)]
        f.write(" ".join(linha) + "\n")
