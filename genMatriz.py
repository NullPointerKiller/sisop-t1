import random

linhas = 500
colunas = 500

with open("matriz 500x500.txt", "w") as f:
    f.write(f"{linhas}x{colunas}\n")

    for i in range(linhas):
        linha = [str(random.randint(0, 1)) for _ in range(colunas)]
        f.write(" ".join(linha) + "\n")