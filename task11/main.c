#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NX 8192
#define NY 8192
#define NT 500
#define DX 0.1f
#define DY 0.1f
#define DT 0.001f
#define VISC 0.1f

typedef struct {
  int nx;
  int ny;
  float **data;
} Grid2D;

Grid2D alocar_grid(int nx, int ny, float valor_base) {
  Grid2D grid = {
      .nx = nx,
      .ny = ny,
      .data = (float **)malloc(nx * sizeof(float *)),
  };

  for (int i = 0; i < nx; i++) {
    grid.data[i] = (float *)malloc(ny * sizeof(float));
  }

  for (int i = 0; i < grid.nx; i++) {
    for (int j = 0; j < grid.ny; j++) {
      grid.data[i][j] = valor_base;
    }
  }
  // Perturbação central
  int cx = grid.nx / 2, cy = grid.ny / 2, r = 4;
  for (int i = cx - r; i <= cx + r; i++) {
    for (int j = cy - r; j <= cy + r; j++) {
      grid.data[i][j] = 10.0f;
    }
  }

  return grid;
}

void calcular_proximo_passo(const Grid2D u_current, const Grid2D u_next,
                            const float nu, const float dt, const float dx,
                            const float dy) {
  const int nx = u_current.nx;
  const int ny = u_current.ny;
  const float idx2 = 1.0f / (dx * dx);
  const float idy2 = 1.0f / (dy * dy);

#pragma omp parallel for
  for (int i = 1; i < nx - 1; i++) {
    for (int j = 1; j < ny - 1; j++) {
      const float laplaciano =
          (u_current.data[i + 1][j] - 2.0f * u_current.data[i][j] +
           u_current.data[i - 1][j]) *
              idx2 +
          (u_current.data[i][j + 1] - 2.0f * u_current.data[i][j] +
           u_current.data[i][j - 1]) *
              idy2;

      u_next.data[i][j] = u_current.data[i][j] + dt * nu * laplaciano;
    }
  }
}

void salvar_metricas_csv(const char *nome_arquivo, const char *tipo_execucao,
                         int nx, int ny, int passos, double tempo) {
  FILE *teste_existencia = fopen(nome_arquivo, "r");
  int precisa_cabecalho = (teste_existencia == NULL);
  if (teste_existencia != NULL) {
    fclose(teste_existencia);
  }

  FILE *f = fopen(nome_arquivo, "a");
  if (f == NULL) {
    fprintf(stderr, "Erro ao abrir o arquivo CSV para escrita.\n");
    return;
  }

  if (precisa_cabecalho) {
    fprintf(f, "tipo_execucao,nx,ny,passos_tempo,tempo_segundos\n");
  }

  fprintf(f, "%s,%d,%d,%d,%f\n", tipo_execucao, nx, ny, passos, tempo);

  fclose(f);
}

int main() {
  const char *label_execucao = "paralelo4";
  const char *nome_csv = "resultados.csv";

  Grid2D u_velA = alocar_grid(NX, NY, 1.0f);
  Grid2D u_velB = alocar_grid(NX, NY, 1.0f);

  omp_set_num_threads(4);

  Grid2D u_atual = u_velA;
  Grid2D u_proximo = u_velB;

  printf("Velocidade inicial no centro: %.2f\n", u_atual.data[NX / 2][NY / 2]);

  double tempo_inicio = omp_get_wtime();

  for (int t = 0; t < NT; t++) {

    calcular_proximo_passo(u_atual, u_proximo, VISC, DT, DX, DY);

    Grid2D temp = u_atual;
    u_atual = u_proximo;
    u_proximo = temp;
  }

  double tempo_fim = omp_get_wtime();
  double tempo_total = tempo_fim - tempo_inicio;

  printf("Velocidade final no centro após %d passos: %f\n", NT,
         u_atual.data[NX / 2][NY / 2]);
  printf("Tempo total gasto: %f segundos\n", tempo_total);

  salvar_metricas_csv(nome_csv, label_execucao, NX, NY, NT, tempo_total);

  return 0;
}
