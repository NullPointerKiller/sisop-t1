#!/bin/sh
# roda todas as matrizes de teste com varias quantidades de threads e confere:
#  - o resultado paralelo e igual ao sequencial
#  - o resultado e igual ao esperado
# uso: sh tests/rodar_testes.sh [executavel]   (chamado pelo make test)

EXEC=${1:-./contador}
THREADS="1 2 3 4 8"
falhas=0

# arquivo:objetos esperados
CASOS="
tests/obrigatorios/exemplo1_5x5.txt:3
tests/obrigatorios/exemplo2_6x8.txt:4
tests/obrigatorios/exemplo3_8x8.txt:5
tests/obrigatorios/exemplo4_9x12.txt:6
tests/obrigatorios/exemplo5_12x12.txt:7
tests/adicionais/a1_zeros_10x10.txt:0
tests/adicionais/a2_coluna_multifaixa_10x10.txt:1
tests/adicionais/a3_diagonal_10x10.txt:1
tests/adicionais/a4_uma_linha_1x30.txt:10
tests/adicionais/a5_tudo_um_10x10.txt:1
"

for caso in $CASOS; do
    arq=${caso%:*}
    esperado=${caso##*:}
    linha="$arq (esperado $esperado):"
    ok=1
    for p in $THREADS; do
        saida=$("$EXEC" "$arq" "$p")
        par=$(echo "$saida" | sed -n 's/.*paralelo, [0-9]* threads): \([0-9]*\).*/\1/p')
        seq=$(echo "$saida" | sed -n 's/.*sequencial): \([0-9]*\).*/\1/p')
        linha="$linha p=$p par=$par seq=$seq;"
        if [ "$par" != "$esperado" ] || [ "$seq" != "$esperado" ]; then
            ok=0
        fi
    done
    if [ $ok -eq 1 ]; then
        echo "APROVADO  $linha"
    else
        echo "FALHOU    $linha"
        falhas=$((falhas + 1))
    fi
done

echo
if [ $falhas -eq 0 ]; then
    echo "Todos os testes aprovados."
else
    echo "$falhas caso(s) falharam."
    exit 1
fi
