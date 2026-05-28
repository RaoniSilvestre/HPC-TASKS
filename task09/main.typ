#import "../template.typ": projeto

#set heading(numbering: "I. ")

#show: projeto.with(titulo: "Regiões críticas nomeadas e travas explicitas")

Para essa atividade, compreendi que é necessário realizar duas implementações:

1. Um programa que faz N inserções em DUAS listas, escolhendo aleatoriamente a cada iteração em qual lista será adicionado um valor, de forma paralela usando openMP.

2. Um programa que faz N inserções em M listas, escolhendo aleatoriamente a cada iteração em qual das M listas será adicionado um valor, de forma paralela usando openMP.


= Lista ligada


Para ambas as implementações, foi necessário criar uma estrutura de `linked list`. Segue a implementação utilizada para os dois códigos:

```c
#include <stdio.h>
#include <stdlib.h>

// Nó da lista, mantém referência pro próximo e qual valor está guardando.
// next == NULL significa que chegou ao fim da lista.
struct Node {
  int val;
  struct Node *next;
} typedef Node;

// Inicializa um novo nó, retorna o ponteiro para o nó inicializado.
Node *new(int val) {
  Node *ptr = (Node *)malloc(sizeof(Node));

  ptr->val = val;
  ptr->next = NULL;

  return ptr;
}

// Varre a lista até o final, e ao fim adiciona um novo nó em last->next.
void append(Node **ptr, int val) {
  Node *last = new(val);

  if (*ptr == NULL) {
    *ptr = last;
    return;
  }

  Node *curr = *ptr;

  while (curr->next != NULL) {
    curr = curr->next;
  }

  curr->next = last;
}

// Enquanto houver itens, imprime eles na tela.
void print(Node *ptr) {
  printf("[");

  Node *curr = ptr;
  while (curr != NULL) {
    printf("%d, ", curr->val);
    curr = curr->next;
  }

  printf("]");
}
```

= Implementação para duas listas

Para a primeira implementação, utilizamos uma região paralela por meio da diretiva `parallel`. Com isso, geramos a seed que será
replicada para cada thread instanciada.

Após isso, iniciamos uma região em que apenas uma thread vai executar, por meio da diretiva `single`. Utilizamos ela para que apenas uma
thread faça a iteração $N$ vezes.

Dentro de cada iteração, criamos uma task para fazer o append na lista de forma aleatorizada. Como existem apenas duas listas, antes de cada operação
de append, criamos uma região crítica, dessa forma conseguimos fazer com que quaisquer duas threads consigam fazer o append das listas l1 e l2 sem barreiras. Apenas duas threads que queiram fazer o append na mesma lista que estarão travadas por meio da região crítica.


```c
#pragma omp parallel
  {
    uint seed = (uint)omp_get_thread_num();

#pragma omp single
    {
      for (int i = 0; i < N; i++) {

        int r = rand_r(&seed);
#pragma omp task firstprivate(i, seed) shared(l1, l2)
        {
          if (r % 2 == 0) {
#pragma omp critical(l1_crit)
            {
              append(&l1, i * 3);
            }
          } else {
#pragma omp critical(l2_crit)
            {
              append(&l2, i * 4);
            }
          }
        }
      }
    }
  }
```
