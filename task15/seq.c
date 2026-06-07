#include "mpi.h"
#include "util.c"
#include <stdio.h>
#include <stdlib.h>

void apply_one_step(float *t1, float *t2, int size) {
  for (int i = 1; i < size - 1; i++) {
    t2[i] = atualizar_ponto(t1, i);
  }
  t2[0] = t2[1];
  t2[size - 1] = t2[size - 2];
}

double simulate_send_recv(int steps, int bar_size) {
  // Inicialização também não entra no tempo de execução.
  float *t1 = init(bar_size, 0);
  float *t2 = init(bar_size, 0);

  // Como vou compilar com o mpicc de qualquer modo, aproveito pra usar a função
  // dele.
  double start = MPI_Wtime();

  for (int j = 0; j < steps; j++) {
    apply_one_step(t1, t2, bar_size);
    swap(&t1, &t2);
  }

  double elapsed = MPI_Wtime() - start;

  // Liberação de memória não entra no tempo de execução.
  free(t1);
  free(t2);

  return elapsed;
}

int main() {
  int sizes[6] = {1e2, 1e4, 1e5, 1e6, 1e7, 1e8};

  int steps = 500;

  for (int i = 0; i < 6; i++) {
    int size = sizes[i];

    double elapsed = simulate_send_recv(steps, size);

    printf(
        "Processamento finalizado de %d iterações... tempo de execução: %f\n",
        size, elapsed);
  }
}
