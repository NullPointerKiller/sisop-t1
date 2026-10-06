#!/bin/sh
# mede o tempo das versoes sequencial e paralela e grava em results/medicoes.csv
# cada execucao do contador mede as duas versoes sobre os mesmos dados
# uso: sh tests/medir.sh [executavel]   (chamado pelo make bench)

EXEC=${1:-./contador}
REPETICOES=5
THREADS="1 2 4 8"
CSV=results/medicoes.csv

# matriz grande para desempenho: gerada com semente fixa (nao vai pro git por causa do tamanho)
GRANDE="tests/grande_2000x2000.txt"
if [ ! -f "$GRANDE" ]; then
    echo "gerando $GRANDE..."
    python3 genMatriz.py 2000 2000 "$GRANDE" 2026 || python genMatriz.py 2000 2000 "$GRANDE" 2026
fi

mkdir -p results
echo "matriz,linhas,colunas,versao,trabalhadores,repeticao,tempo_ms,objetos,resultado_correto" > "$CSV"

medir() {
    arq="$1"
    dim=$(head -n 1 "$arq" | tr -d '\r')
    linhas=${dim%x*}
    colunas=${dim#*x}
    for p in $THREADS; do
        r=1
        while [ $r -le $REPETICOES ]; do
            saida=$("$EXEC" "$arq" "$p")
            par=$(echo "$saida" | sed -n 's/.*paralelo, [0-9]* threads): \([0-9]*\) (\([0-9.]*\) s).*/\1 \2/p')
            seq=$(echo "$saida" | sed -n 's/.*sequencial): \([0-9]*\) (\([0-9.]*\) s).*/\1 \2/p')
            objPar=${par% *}; tPar=${par#* }
            objSeq=${seq% *}; tSeq=${seq#* }
            certo=false
            [ -n "$objPar" ] && [ "$objPar" = "$objSeq" ] && certo=true
            msPar=$(awk "BEGIN { printf \"%.3f\", $tPar * 1000 }")
            msSeq=$(awk "BEGIN { printf \"%.3f\", $tSeq * 1000 }")
            echo "$arq,$linhas,$colunas,sequencial,1,$p-$r,$msSeq,$objSeq,true" >> "$CSV"
            echo "$arq,$linhas,$colunas,paralela,$p,$r,$msPar,$objPar,$certo" >> "$CSV"
            echo "$arq p=$p rep=$r: seq ${msSeq} ms, par ${msPar} ms, objetos $objSeq/$objPar"
            r=$((r + 1))
        done
    done
}

medir "matriz 500x500.txt"
medir "$GRANDE"

echo "medicoes gravadas em $CSV"
