#import "../template.typ": projeto

#set heading(numbering: "I. ")
#show: projeto.with(titulo: "Comunicação MPI")

#import "@preview/cetz:0.5.2"
#import "@preview/cetz-plot:0.1.4": plot


= Metodologia

O experimento consistiu em  utilizar um programa que para cada tipo de comunicação, são executadas mil iterações de comunicação para cada tamanho de carga, que varia de 8 Bytes até 512KB.

Nesse contexto, foram testados os seguintes métodos de comunicação:

- `MPI_Send`
- `MPI_Ssend`
- `MPI_Bsend`
- `MPI_Rsend`

= Resultados

#let mpi-send = (
  (3, 25.45),
  (5, 74.52),
  (7, 283.34),
  (9, 1112.24),
  (11, 2933.73),
  (13, 4819.25),
  (15, 7193.64),
  (17, 11118.27),
  (19, 13144.85),
)
#let mpi-ssend = (
  (3, 14.61),
  (5, 42.61),
  (7, 197.86),
  (9, 675.23),
  (11, 2285.64),
  (13, 4476.81),
  (15, 7007.91),
  (17, 11149.41),
  (19, 12782.66),
)
#let mpi-bsend = (
  (3, 21.37),
  (5, 63.23),
  (7, 260.24),
  (9, 1002.08),
  (11, 2335.39),
  (13, 4349.94),
  (15, 5675.68),
  (17, 7854.03),
  (19, 8187.26),
)
#let mpi-rsend = (
  (3, 17.55),
  (5, 51.80),
  (7, 138.70),
  (9, 894.14),
  (11, 2089.48),
  (13, 4414.35),
  (15, 6630.12),
  (17, 10287.45),
  (19, 12082.61),
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
)

#let to-log10(dados) = dados.map(p => (p.at(0), calc.log(p.at(1), base: 10)))
#let y-ticks = (
  (1, [10]),
  (2, [100]),
  (3, [1.000]),
  (4, [10.000]),
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

No mais, foi observado que o método `MPI_Send` foi o vencedor absoluto. Isso pode ser explicado pois a implementação do OpenMPI que vai decidir se para aquele caso ele vai utilizar um buffer ou não, e pelo visto ele conseguiu decidir bem em todos os casos.


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
    /* Escreve o cabeçalho do CSV */
    fprintf(arquivo_csv, "Metodo,Tamanho_Bytes,Tempo_Segundos,Banda_MBs\n");
  }

  const int TAMANHO_MAXIMO = 1048576;
  const int ITERACOES = 10000;

  executar_ping_pong(rank, "MPI_Send", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);
  executar_ping_pong(rank, "MPI_Ssend", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);
  executar_ping_pong(rank, "MPI_Bsend", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);
  executar_ping_pong(rank, "MPI_Rsend", TAMANHO_MAXIMO, ITERACOES, arquivo_csv);

  MPI_Finalize();
  return 0;
}
```
