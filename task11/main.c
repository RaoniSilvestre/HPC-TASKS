#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NX 1000 // Tamanho da malha em X
#define NY 1000 // Tamanho da malha em Y
#define NT 2000 // Número de passos de tempo
#define NU 0.05 // Viscosidade cinemática
#define DX 0.1  // Resolução espacial X
#define DY 0.1  // Resolução espacial Y
#define DT 0.04 // Passo de tempo (deve respeitar estabilidade de Courant)

// Função para inicializar o campo (estável ou com perturbação)
void inicializar_campo(double *u, int com_perturbacao) {
  for (int i = 0; i < NX; i++) {
    for (int j = 0; j < NY; j++) {
      u[i * NY + j] = 0.0; // Fluido inicialmente parado (estável)
    }
  }

  // Insere uma pequena perturbação no centro do domínio
  if (com_perturbacao) {
    int cx = NX / 2;
    int cy = NY / 2;
    int raio = 20;
    for (int i = cx - raio; i < cx + raio; i++) {
      for (int j = cy - raio; j < cy + raio; j++) {
        if ((i - cx) * (i - cx) + (j - cy) * (j - cy) < raio * raio) {
          u[i * NY + j] = 1.0; // Pico de velocidade
        }
      }
    }
  }
}

// Simulação sem paralelismo (Referência)
double simular_serial(double *u, double *un) {
  double inicio = omp_get_wtime();
  for (int t = 0; t < NT; t++) {
    for (int i = 1; i < NX - 1; i++) {
      for (int j = 1; j < NY - 1; j++) {
        double d2udx2 =
            (u[(i + 1) * NY + j] - 2.0 * u[i * NY + j] + u[(i - 1) * NY + j]) /
            (DX * DX);
        double d2udy2 =
            (u[i * NY + j + 1] - 2.0 * u[i * NY + j] + u[i * NY + j - 1]) /
            (DY * DY);
        un[i * NY + j] = u[i * NY + j] + NU * DT * (d2udx2 + d2udy2);
      }
    }
    // Atualiza a malha
    for (int i = 1; i < NX - 1; i++) {
      for (int j = 1; j < NY - 1; j++) {
        u[i * NY + j] = un[i * NY + j];
      }
    }
  }
  return omp_get_wtime() - inicio;
}

// Simulação com OpenMP testando clauses
double simular_omp(double *u, double *un, const char *tipo_schedule,
                   int usar_collapse) {
  double inicio = omp_get_wtime();
  for (int t = 0; t < NT; t++) {

    if (usar_collapse) {
#pragma omp parallel for collapse(2) schedule(runtime)
      for (int i = 1; i < NX - 1; i++) {
        for (int j = 1; j < NY - 1; j++) {
          double d2udx2 = (u[(i + 1) * NY + j] - 2.0 * u[i * NY + j] +
                           u[(i - 1) * NY + j]) /
                          (DX * DX);
          double d2udy2 =
              (u[i * NY + j + 1] - 2.0 * u[i * NY + j] + u[i * NY + j - 1]) /
              (DY * DY);
          un[i * NY + j] = u[i * NY + j] + NU * DT * (d2udx2 + d2udy2);
        }
      }
    } else {
#pragma omp parallel for schedule(runtime)
      for (int i = 1; i < NX - 1; i++) {
        for (int j = 1; j < NY - 1; j++) {
          double d2udx2 = (u[(i + 1) * NY + j] - 2.0 * u[i * NY + j] +
                           u[(i - 1) * NY + j]) /
                          (DX * DX);
          double d2udy2 =
              (u[i * NY + j + 1] - 2.0 * u[i * NY + j] + u[i * NY + j - 1]) /
              (DY * DY);
          un[i * NY + j] = u[i * NY + j] + NU * DT * (d2udx2 + d2udy2);
        }
      }
    }

// Atualização paralela
#pragma omp parallel for schedule(runtime)
    for (int i = 1; i < NX - 1; i++) {
      for (int j = 1; j < NY - 1; j++) {
        u[i * NY + j] = un[i * NY + j];
      }
    }
  }
  return omp_get_wtime() - inicio;
}

int main() {
  double *u = (double *)malloc(NX * NY * sizeof(double));
  double *un = (double *)malloc(NX * NY * sizeof(double));

  printf("--- Validacao Inicial ---\n");
  inicializar_campo(u, 0); // Sem perturbação
  printf("Fluido inicializado em repouso absoluto.\n");
  simular_serial(u, un);
  printf(
      "Apos simular %d iteracoes, valor max no centro: %f (Esperado: 0.0)\n\n",
      NT, u[(NX / 2) * NY + (NY / 2)]);

  printf("--- Testes de Desempenho OpenMP ---\n");
  printf("Malha: %dx%d | Iteracoes: %d | Threads: %d\n\n", NX, NY, NT,
         omp_get_max_threads());

  // 1. Serial
  inicializar_campo(u, 1);
  double t_serial = simular_serial(u, un);
  printf("Tempo Serial: \t\t\t%.4f segundos\n", t_serial);

  omp_set_schedule(omp_sched_static, 0);
  inicializar_campo(u, 1);
  double t_static = simular_omp(u, un, "static", 0);
  printf("OpenMP schedule(static): \t%.4f segundos\n", t_static);

  omp_set_schedule(omp_sched_dynamic, 16); // Chunk de 16
  inicializar_campo(u, 1);
  double t_dynamic = simular_omp(u, un, "dynamic, 16", 0);
  printf("OpenMP schedule(dynamic,16):\t%.4f segundos\n", t_dynamic);

  omp_set_schedule(omp_sched_static, 0);
  inicializar_campo(u, 1);
  double t_collapse = simular_omp(u, un, "static", 1);
  printf("OpenMP static + collapse(2): \t%.4f segundos\n", t_collapse);

  return 0;
}
