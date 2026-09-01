#import "../template.typ": projeto

#set heading(numbering: "I. ")
#show: projeto.with(titulo: "Comunicação MPI")

#import "@preview/cetz:0.5.2"
#import "@preview/cetz-plot:0.1.4": plot


= Metodologia

O experimento consistiu em  utilizar um programa que para cada tipo de comunicação, são executadas cem iterações de comunicação para cada tamanho de carga, que varia de 8 Bytes até 32MB.

Nesse contexto, foram testados os seguintes métodos de comunicação:

- `MPI_Send`
- `MPI_Ssend`
- `MPI_Bsend`
- `MPI_Rsend`

= Resultados

#let mpi-send = (
  (3, 17.26),
  (5, 59.04),
  (7, 231.03),
  (9, 895.40),
  (11, 1016.95),
  (13, 4968.65),
  (15, 7241.63),
  (17, 11938.09),
  (19, 12145.17),
  (21, 10525.79),
  (23, 7353.10),
  (25, 6775.65),
)
#let mpi-ssend = (
  (3, 10.48),
  (5, 45.75),
  (7, 181.45),
  (9, 693.05),
  (11, 1980.61),
  (13, 4913.06),
  (15, 7099.30),
  (17, 11987.87),
  (19, 13251.97),
  (21, 9947.50),
  (23, 7622.90),
  (25, 6392.94),
)
#let mpi-bsend = (
  (3, 11.43),
  (5, 31.29),
  (7, 121.14),
  (9, 380.02),
  (11, 1235.01),
  (13, 2979.39),
  (15, 4734.82),
  (17, 6291.60),
  (19, 7502.15),
  (21, 5502.82),
  (23, 3843.56),
  (25, 3813.06),
)
#let mpi-rsend = (
  (3, 17.56),
  (5, 49.75),
  (7, 208.06),
  (9, 790.21),
  (11, 2503.69),
  (13, 5177.46),
  (15, 7365.32),
  (17, 10981.99),
  (19, 12622.67),
  (21, 12791.43),
  (23, 6831.37),
  (25, 6972.46),
)

#let x-ticks = (
  (3, [8B]),
  (5, [32B]),
  (7, [128B]),
  (9, [512B]),
  (11, [2K]),
  (13, [8K]),
  (15, [32K]),
  (17, [128K]),
  (19, [512K]),
  (21, [2M]),
  (23, [8M]),
  (25, [32M]),
)

#let to-log10(dados) = dados.map(p => (p.at(0), calc.log(p.at(1), base: 10)))
#let y-ticks = (
  (1, [10]),
  (2, [100]),
  (3, [1.000]),
  (4, [10.000]),
  (5, [100.000]),
)

#align(center)[
  #cetz.canvas({
    plot.plot(
      size: (14, 8),
      x-label: "Tamanho da Mensagem (Log2 de Bytes)",
      y-label: "Largura de Banda (MB/s)",
      legend: "inner-north-west",
      x-ticks: x-ticks,
      x-tick-step: none,
      y-ticks: y-ticks,
      y-tick-step: none,
      {
        plot.add(to-log10(mpi-send), label: "MPI_Send", style: (stroke: blue + 1.5pt), mark: "square")
        plot.add(to-log10(mpi-ssend), label: "MPI_Ssend", style: (stroke: orange + 1.5pt), mark: "square")
        plot.add(to-log10(mpi-bsend), label: "MPI_Bsend", style: (stroke: green + 1.5pt), mark: "triangle")
        plot.add(to-log10(mpi-rsend), label: "MPI_Rsend", style: (stroke: red + 1.5pt), mark: "diamond")
      },
    )
  })
]

= Análise

Visto que esse programa roda localmente em uma única máquina, não envolveu o uso de uma rede externa que faça com que os processos se comuniquem. Isso gerou resultados interessantes
onde foi possível transitar até 13Gb/s entre os dois processos. Isso não seria esperado caso fosse utilizado uma rede Gigabit por exemplo. A comunicação no último caso seria limitada
pela própria capacidade de comunicação da infraestrutura de rede.

No mais, foi observado que o método `MPI_Rsend` foi o vencedor geral. Isso pode ser explicado pelo fato de que esse método assume que já existe alguem escutando a mensagem e também
não aloca buffers intermediários. Acaba sendo um método mais perigoso mas também é mais eficiente.


= Anexo

```c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void executar_ping_pong(int rank, const char *metodo, int tamanho_maximo,
                        int iteracoes, FILE *arquivo_csv) {
  if (rank == 0) {
    printf("\n--- Testando %s ---\n", metodo);
    printf("%-15s %-20s %-20s\n", "Tamanho (B)", "Tempo Medio (s)",
           "Banda (MB/s)");
  }

  char *bsend_buffer = NULL;
  if (strcmp(metodo, "MPI_Bsend") == 0) {
    int tamanho_buffer = tamanho_maximo + MPI_BSEND_OVERHEAD;
    bsend_buffer = (char *)malloc(tamanho_buffer * sizeof(char));
    MPI_Buffer_attach(bsend_buffer, tamanho_buffer);
  }

  for (int tamanho = 8; tamanho <= tamanho_maximo; tamanho *= 4) {
    char *mensagem = (char *)malloc(tamanho * sizeof(char));
    memset(mensagem, 'x', tamanho);
    MPI_Status status;

    MPI_Barrier(MPI_COMM_WORLD);
    double tempo_inicio = MPI_Wtime();

    for (int i = 0; i < iteracoes; ++i) {
      if (rank == 0) {
        if (strcmp(metodo, "MPI_Send") == 0) {
          MPI_Send(mensagem, tamanho, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        } else if (strcmp(metodo, "MPI_Ssend") == 0) {
          MPI_Ssend(mensagem, tamanho, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        } else if (strcmp(metodo, "MPI_Bsend") == 0) {
          MPI_Bsend(mensagem, tamanho, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        } else if (strcmp(metodo, "MPI_Rsend") == 0) {
          MPI_Rsend(mensagem, tamanho, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        }

        MPI_Recv(mensagem, tamanho, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);

      } else if (rank == 1) {
        MPI_Recv(mensagem, tamanho, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

        if (strcmp(metodo, "MPI_Send") == 0) {
          MPI_Send(mensagem, tamanho, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        } else if (strcmp(metodo, "MPI_Ssend") == 0) {
          MPI_Ssend(mensagem, tamanho, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        } else if (strcmp(metodo, "MPI_Bsend") == 0) {
          MPI_Bsend(mensagem, tamanho, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        } else if (strcmp(metodo, "MPI_Rsend") == 0) {
          MPI_Rsend(mensagem, tamanho, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
      }
    }

    double tempo_fim = MPI_Wtime();

    if (rank == 0) {
      double tempo_medio = (tempo_fim - tempo_inicio) / (2.0 * iteracoes);
      double banda_mb = (tamanho / (1024.0 * 1024.0)) / tempo_medio;

      printf("%-15d %-20.8f %-20.2f\n", tamanho, tempo_medio, banda_mb);
      fprintf(arquivo_csv, "%s,%d,%.8f,%.2f\n", metodo, tamanho, tempo_medio,
              banda_mb);
    }

    free(mensagem);
  }

  if (strcmp(metodo, "MPI_Bsend") == 0) {
    int tamanho_desalocado;
    char *ponteiro_buffer;
    MPI_Buffer_detach(&ponteiro_buffer, &tamanho_desalocado);
    free(bsend_buffer);
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != 2) {
    if (rank == 0) {
      fprintf(stderr, "Erro: Este programa deve ser executado com exatamente 2 "
                      "processos.\n");
    }

    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  FILE *arquivo_csv = NULL;

  if (rank == 0) {
    arquivo_csv = fopen("resultados_mpi.csv", "w");
    if (arquivo_csv == NULL) {
      fprintf(stderr, "Erro ao criar o arquivo CSV.\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    fprintf(arquivo_csv, "Metodo,Tamanho_Bytes,Tempo_Segundos,Banda_MBs\n");
  }

  const int TAMANHO_MAXIMO = 1e8;
  const int ITERACOES = 100;

  executar_ping_pong(rank, "MPI_Send", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);
  executar_ping_pong(rank, "MPI_Ssend", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);
  executar_ping_pong(rank, "MPI_Bsend", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);
  executar_ping_pong(rank, "MPI_Rsend", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);

  MPI_Finalize();
  return 0;
}
```
