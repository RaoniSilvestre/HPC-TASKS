#import "../template.typ": projeto

#set heading(numbering: "I. ")
#show: projeto.with(titulo: "Escalonador Líder trabalhador")

#import "@preview/cetz:0.5.2"
#import "@preview/cetz-plot:0.1.4": plot

= Metodologia

A implementação utiliza o padrão Líder-Trabalhador com escalonamento dinâmico. O processo líder é responsável por dividir a busca em lotes (chunks) e enviar uma tarefa inicial
(contendo o início e fim de um intervalo) para cada trabalhador. À medida que um trabalhador conclui seu lote, ele devolve a contagem de primos encontrados. O líder recebe esse
resultado parcial, soma ao total e imediatamente aloca um novo intervalo para esse trabalhador. Esse fluxo de distribuição sob demanda garante o balanceamento de carga, mantendo
todos os processos ocupados até que o espaço de busca seja esgotado, momento em que o líder envia um sinal de encerramento aos trabalhadores.

= Resultados

O gráfico abaixo ilustra o tempo de execução (em segundos) em função do número de núcleos utilizados. Para fins de comparação, também é apresentada a curva de tempo ideal (considerando o tempo inicial de 4 núcleos reduzindo perfeitamente pela metade a cada dobra na quantidade de processadores).

#align(center)[
  #cetz.canvas({
    let time-data = (
      (4, 315.388),
      (8, 209.604),
      (16, 104.079),
      (32, 51.879),
      (64, 25.917),
      (128, 12.989),
    )

    // Dados de tempo ideal (T(p) = T(4) * 4 / p)
    let ideal-time-data = (
      (4, 315.388),
      (8, 157.694),
      (16, 78.847),
      (32, 39.423),
      (64, 19.711),
      (128, 9.855),
    )

    plot.plot(
      size: (12, 6),
      x-label: "Número de Núcleos",
      y-label: "Tempo de Execução (s)",
      x-tick-step: 16,
      {
        plot.add(
          ideal-time-data,
          label: "Tempo Ideal",
          style: (stroke: (paint: gray, dash: "dashed")),
        )
        plot.add(
          time-data,
          label: "Tempo Real",
          mark: "o",
          style: (stroke: (paint: red, thickness: 2pt)),
        )
      },
    )
  })
]
