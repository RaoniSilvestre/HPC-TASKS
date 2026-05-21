# Tarefa 6 - Calculo pi paralelo		

Implemente em C a estimativa estocástica de π. Paralelize com 

```c
#pragma omp parallel for 
```

e explique o resultado incorreto. Corrija a condição de corrida utilizando o 

```c
#pragma omp critical 
```

e reestruturando com 

```c
#pragma omp parallel 
```

seguido de 

```c
#pragma omp for 
```

e aplicando as cláusulas private, firstprivate, lastprivate e shared. Teste diferentes combinações e explique como cada cláusula afeta o comportamento do programa.
Comente também como a cláusula default(none) pode ajudar a tornar o escopo mais claro em programas complexos.
