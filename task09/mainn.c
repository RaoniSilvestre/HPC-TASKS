#include "ll.c"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int N = 200;
int M = 10;

int main() {

  // M listas
  Node **listas = (Node **)malloc(M * sizeof(Node *));
  // M locks
  omp_lock_t *locks = (omp_lock_t *)malloc(M * sizeof(omp_lock_t));

  for (int i = 0; i < M; i++) {
    listas[i] = NULL;
    // Inicializa os N locks
    omp_init_lock(&locks[i]);
  }

#pragma omp parallel
  {
    uint seed = (uint)omp_get_thread_num();
#pragma omp single
    {

      for (int i = 0; i < N; i++) {
#pragma omp task firstprivate(i) shared(listas, locks, M, seed)
        {
          int lista_escolhida = rand_r(&seed) % M;

          omp_set_lock(&locks[lista_escolhida]);
          append(&listas[lista_escolhida], i);
          omp_unset_lock(&locks[lista_escolhida]);
        }
      }
    }
  }

  for (int i = 0; i < M; i++) {
    printf("\nLista %d: ", i);
    print(listas[i]);
  }

  return 0;
}
