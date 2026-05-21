# Tarefa 4: Memory Bound X Compute Bound		

Implemente dois programas paralelos em C com OpenMP: um limitado por memória, com somas simples em vetores, e outro limitado por CPU, com cálculos matemáticos intensivos. Paralelize com 

```c
#pragma omp parallel for 
```

e avalie o desempenho. 

Analise quando o desempenho melhora, estabiliza ou piora, e reflita sobre como o multithreading de hardware pode ajudar em programas memory-bound, mas
atrapalhar em programas compute-bound pela competição por recursos.

Explique quais são as melhores métricas para avaliar cada caso e como cada uma pode representar os diferentes aspectos do problema.
