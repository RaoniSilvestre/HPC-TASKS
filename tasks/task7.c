#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implemente um programa em C que crie uma lista encadeada de nós, cada um
// contendo o nome de um arquivo fictício. Dentro de uma região paralela,
// percorra a lista e crie uma tarefa com #pragma omp task para processar cada
// nó. Cada tarefa deve imprimir o nome do arquivo e o identificador da thread
// que a executou. Após executar o programa, reflita: todos os nós foram
// processados? Algum foi processado mais de uma vez ou ignorado? O
// comportamento muda entre execuções? Como garantir que cada nó seja processado
// uma única vez e por apenas uma tarefa?

struct Node {
  char *file;
  struct Node *next;
} typedef Node;

Node *create_node(char *filename) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (!new_node)
    return NULL;

  new_node->file = strdup(filename);
  new_node->next = NULL;
  return new_node;
}

Node *append_node(Node *head, char *filename) {
  Node *new_node = create_node(filename);
  if (head == NULL) {
    head = new_node;
    return head;
  }
  Node *temp = head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = new_node;

  return temp;
}

int size = 1e2;

#define MAX_FILENAME 50

int main() {

  Node *list = create_node("nó raiz");
  Node *temp = list;

  for (int i = 0; i < size; i++) {
    char buffer[MAX_FILENAME];
    snprintf(buffer, sizeof(buffer), "arquivo_%d.txt", i);
    temp = append_node(temp, buffer);
  }

  printf("Iniciando processamento paralelo:\n");

  // define a região paralela
#pragma omp parallel
  {
    // Define que apenas uma thread deve fazer iterar sobre a lista de nós
#pragma omp single
    {
      Node *current = list;
      while (current != NULL) {
        // Define que a primeira que chegar aqui e somente ela, deve executar
        // esse bloco de código
#pragma omp task firstprivate(current)
        {
          printf("Processando: %s | Thread ID: %d\n", current->file,
                 omp_get_thread_num());
        }
        current = current->next;
      }
    }
  }

  return 0;
}
