
// Tarefa 5 : Números primos de 24 / 03 / 2026 às 00h00 a 31 / 03 /2026 às 23h59
//  Implemente um programa em C que conte quantos números primos existem
// entre 2 e um valor máximo n.Depois, paralelize o laço principal usando a
// diretiva
//
// #pragma omp parallel for
//
// sem alterar a lógica original. compare o tempo de execução e os
// resultados das versões sequencial e paralela. observe possíveis
// diferenças no resultado e no desempenho, e reflita sobre os desafios
// iniciais da programação paralela, como correção e distribuição de carga.

#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool is_prime(int n) {
  if (n == 2) {
    return true;
  }
  if (n < 2 || n % 2 == 0) {
    return false;
  }

  int k = n / 2;

  for (int i = 3; i < k; i++) {
    if (n % i == 0) {
      return false;
    }
  }

  return true;
}

int size = 1e6;

int main(void) {
  int cnt = 0;

  double inicio, fim;

  inicio = omp_get_wtime();

  omp_set_num_threads(12);
#pragma omp parallel for
  for (int i = 2; i < size; i++) {
    if (is_prime(i)) {
#pragma omp atomic
      cnt++;
    }
  }

  fim = omp_get_wtime();

  printf("SAFE PARALLEL EXECUTION\n");
  printf("Elapsed wall time:  %f seconds\n", fim - inicio);
  printf("%d\n", cnt);

  return EXIT_SUCCESS;
}
