# Tarefa 7: Processamento Paralelo de Lista Encadeada com OpenMP Task		

Implemente um programa em C que crie uma lista encadeada de nós, cada um contendo o nome de um arquivo fictício. Dentro de uma região paralela, percorra a
lista e crie uma tarefa com `#pragma omp task` para processar cada nó. Cada tarefa deve imprimir o nome do arquivo e o identificador da thread que a executou.
Após executar o programa, reflita: todos os nós foram processados? Algum foi processado mais de uma vez ou ignorado? O comportamento muda entre execuções?
Como garantir que cada nó seja processado uma única vez e por apenas uma tarefa?
