#include "mpi.h"
#include "util.c"
#include <stdio.h>
#include <stdlib.h>

void apply_one_step(float *t1, float *t2, int size) {
  for (int i = 1; i <= size; i++) {
    t2[i] = atualizar_ponto(t1, i);
  }
}

void communicate(int rank, int esq, int dir, float *bar, int bar_size) {
  // Criamos um vetor para rastrear até 4 requisições (2 envios + 2
  // recebimentos)
  MPI_Request reqs[4];
  int req_count = 0;

  // Postar os recebimentos
  if (esq != MPI_PROC_NULL) {
    MPI_Irecv(&bar[0], 1, MPI_FLOAT, esq, 0, MPI_COMM_WORLD,
              &reqs[req_count++]);
  }
  if (dir != MPI_PROC_NULL) {
    MPI_Irecv(&bar[bar_size + 1], 1, MPI_FLOAT, dir, 1, MPI_COMM_WORLD,
              &reqs[req_count++]);
  }

  // Postar os envios
  if (esq != MPI_PROC_NULL) {
    MPI_Isend(&bar[1], 1, MPI_FLOAT, esq, 1, MPI_COMM_WORLD,
              &reqs[req_count++]);
  }
  if (dir != MPI_PROC_NULL) {
    MPI_Isend(&bar[bar_size], 1, MPI_FLOAT, dir, 0, MPI_COMM_WORLD,
              &reqs[req_count++]);
  }

  // Sincroniza
  MPI_Waitall(req_count, reqs, MPI_STATUSES_IGNORE);
}

double simulate_isend_irecv(int steps, int local_bar_size, int rank,
                            int world_size) {
  // Inicialização também não entra no tempo de execução.
  float *t1 = init(local_bar_size + 2, rank);
  float *t2 = init(local_bar_size + 2, rank);

  // Vizinho esquerdo
  int v_esq = rank == 0 ? MPI_PROC_NULL : rank - 1;
  // Vizinho direito
  int v_dir = rank == world_size - 1 ? MPI_PROC_NULL : rank + 1;

  MPI_Barrier(MPI_COMM_WORLD);

  printf("Iniciando processamento de %d passos no processo %d .\n", steps,
         rank);

  // Como vou compilar com o mpicc de qualquer modo, aproveito pra usar a função
  // dele.
  double start = MPI_Wtime();

  for (int step = 0; step < steps; step++) {
    // Comunica a ponta de cada processo
    communicate(rank, v_esq, v_dir, t1, local_bar_size);

    // Processa 1 step
    apply_one_step(t1, t2, local_bar_size);

    swap(&t1, &t2);

    if (rank == 0) {
      t1[0] = 100.0;
    }
  }
  double elapsed = MPI_Wtime() - start;

  // Liberação de memória não entra no tempo de execução.
  free(t1);
  free(t2);

  return elapsed;
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size < 2) {
    if (rank == 0) {
      printf("Utilize pelo menos 2 processos MPI para rodar esse programa.\n");
    }
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int bar_size = 1e8;
  int bar_per_process = bar_size / size;

  int steps = 500;

  if (rank == 0) {
    printf("Simulando difusao de calor na barra 1D...\n");
    printf("Tamanho global: %d pontos | Processos: %d | Passos: %d\n", bar_size,
           size, steps);
    printf("----------------------------------------------------------\n");
  }

  double elapsed = simulate_isend_irecv(steps, bar_per_process, rank, size);

  if (rank == 0) {
    printf(
        "Processamento finalizado de %d iterações... tempo de execução: %f\n",
        steps, elapsed);
  }

  MPI_Finalize();

  return 0;
}
