# gera os graficos de tempo, aceleracao e eficiencia a partir de results/medicoes.csv
# uso: python3 results/graficos.py   (precisa do matplotlib)
import csv
import os
import statistics

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

PASTA = os.path.dirname(os.path.abspath(__file__))
THREADS = [1, 2, 4, 8]
CORES = ["#2a78d6", "#eb6834"]

linhas = list(csv.DictReader(open(os.path.join(PASTA, "medicoes.csv"))))
matrizes = list(dict.fromkeys(l["matriz"] for l in linhas))


def nome(arq):
    return os.path.basename(arq).replace("matriz ", "").replace("grande_", "").replace(".txt", "")


def tempos(arq, versao, p=None):
    return [float(l["tempo_ms"]) for l in linhas
            if l["matriz"] == arq and l["versao"] == versao
            and (p is None or l["trabalhadores"] == str(p))]


def eixos(ax, titulo, rotulo_y):
    ax.set_title(titulo, fontsize=13, loc="left", color="#222")
    ax.set_xlabel("Trabalhadores (threads)", color="#555")
    ax.set_ylabel(rotulo_y, color="#555")
    ax.grid(axis="y", color="#e5e5e5", lw=0.8)
    ax.set_axisbelow(True)
    for lado in ["top", "right"]:
        ax.spines[lado].set_visible(False)
    for lado in ["left", "bottom"]:
        ax.spines[lado].set_color("#bbb")
    ax.tick_params(colors="#555")


# tempo: um painel por matriz (escalas muito diferentes), mediana com barras min-max
fig, paineis = plt.subplots(1, len(matrizes), figsize=(10, 4.5), dpi=150)
for ax, arq, cor in zip(paineis, matrizes, CORES):
    grupos = [tempos(arq, "sequencial")] + [tempos(arq, "paralela", p) for p in THREADS]
    med = [statistics.median(g) for g in grupos]
    erro = [[m - min(g) for m, g in zip(med, grupos)], [max(g) - m for m, g in zip(med, grupos)]]
    ax.bar(range(len(grupos)), med, 0.6, color=["#8a8a8a"] + [cor] * len(THREADS),
           yerr=erro, error_kw=dict(ecolor="#444", lw=1, capsize=3))
    ax.set_xticks(range(len(grupos)))
    ax.set_xticklabels(["Seq."] + [str(p) for p in THREADS])
    eixos(ax, "Matriz " + nome(arq), "Tempo (ms)")
fig.suptitle("Tempo da contagem (mediana; barras = mín-máx)", x=0.01, ha="left", fontsize=13)
fig.tight_layout()
fig.savefig(os.path.join(PASTA, "grafico-tempo.png"))
plt.close(fig)

# aceleracao e eficiencia
for arquivo, titulo, rotulo, ideal, limite in [
        ("grafico-aceleracao.png", "Aceleração S(p) = Tseq / Tpar(p)", "Aceleração", lambda p: p, 8.5),
        ("grafico-eficiencia.png", "Eficiência E(p) = S(p) / p", "Eficiência", lambda p: 1, 1.1)]:
    fig, ax = plt.subplots(figsize=(8, 4.5), dpi=150)
    ax.plot(THREADS, [ideal(p) for p in THREADS], ls="--", color="#999", lw=1.5, label="Ideal")
    for arq, cor in zip(matrizes, CORES):
        tseq = statistics.median(tempos(arq, "sequencial"))
        s = [tseq / statistics.median(tempos(arq, "paralela", p)) for p in THREADS]
        y = s if "acel" in arquivo else [v / p for v, p in zip(s, THREADS)]
        ax.plot(THREADS, y, color=cor, lw=2, marker="o", ms=8, label=nome(arq))
        ax.annotate(f"{y[-1]:.2f}".replace(".", ","), (THREADS[-1], y[-1]),
                    textcoords="offset points", xytext=(8, -4), color="#333", fontsize=9)
    ax.set_xticks(THREADS)
    ax.set_ylim(0, limite)
    eixos(ax, titulo, rotulo)
    ax.legend(frameon=False)
    fig.tight_layout()
    fig.savefig(os.path.join(PASTA, arquivo))
    plt.close(fig)

print("graficos gerados em", PASTA)
