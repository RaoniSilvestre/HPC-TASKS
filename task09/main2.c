#include "ll.c"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int N = 1e2;

int main() {
  Node *l1 = NULL;
  Node *l2 = NULL;

#pragma omp parallel
  {
    uint seed = (uint)omp_get_thread_num();

#pragma omp single
    {
      for (int i = 0; i < N; i++) {

        int r = rand_r(&seed);
#pragma omp task firstprivate(i) shared(l1, l2, seed)
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

  printf("L1: ");
  print(l1);
  printf("\nL2: ");
  print(l2);

  return 0;
}
