#include <stdio.h>
#include <stdlib.h>

struct Node {
  int val;
  struct Node *next;
} typedef Node;

Node *new(int val) {
  Node *ptr = (Node *)malloc(sizeof(Node));

  ptr->val = val;
  ptr->next = NULL;

  return ptr;
}

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

void print(Node *ptr) {
  printf("[");

  Node *curr = ptr;
  while (curr != NULL) {
    printf("%d, ", curr->val);
    curr = curr->next;
  }

  printf("]");
}
