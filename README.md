#MPI & OpenMP

O objetivo do trabalho é implementar uma versão paralela híbrida da
ordenação de vários vetores em um saco de trabalho (vetor de vetores)
usando as ferramentas MPI e OpenMP.

Para a exploração do paralelismo sem memória compartilhada deverá ser
utilizada uma versão adaptada do modelo mestre/escravo implementado
com MPI no primeiro trabalho, com o propósito de distribuir os vetores
a serem ordenados nos diferentes nós dos cluster. Pra isso será usado
apenas um processo pesado MPI por nó. A ordem original dos vetores no
saco deve ser mantida.

Para a exploração do paralelismo com memória compartilhada dentro de
cada nó deverá ser implementada uma ordenação de vetores seguindo o
modelo workpool em OpenMP (todas as threads ordenam e o controle é
feito por uma estrutura de dados, sem mestre).  Deve ser utilizada
pelo menos uma thread para cada núcleo do nó e o programa deve receber
como entrada o algoritmo de ordenação a ser utilizado (quick sort -
função da linguagem C - ou o bubble sort - código abaixo).

Após implementado, o programa deve ser executado em quatro nós do
cluster atlantica na ordenação de 10.000 vetores de 100.000 elementos.
Cada grupo (de dois integrantes) deve entregar um relatório em .pdf de
uma página com a análise dos resultados e uma página com o código
(seguir modelo proposto).

Os itens para avaliação são:

- foco na análise dos resultados se utilizando das métricas de speed-up e
eficiência

- cálculo do speed up e da eficiência para o caso de teste e número de
processadores;

- análise das diferentes formas de alocar o processo mestre do MPI

- comparação do fator de aceleração (speed-up) entre os diferentes algoritmos
de ordenação (quick sort e bubble sort)

- comparação do fator de aceleração (speed-up) da versão MPI pura (modelo
mestre/escravo) e justificativa de ter ficado melhor/pior

- análise do número de horas máquina usadas pelo grupo no LAD durante o
desenvolvimento do trabalho (feita pelo professor).

