#include <stdio.h>
#include <stdlib.h>

float atualizar_ponto(float *arr, int i) {
  return arr[i] + (0.4 * (arr[i - 1] - (2 * arr[i]) + arr[i + 1]));
}

void print_arr(float *arr, int size) {
  if (size == 0) {
    printf("[]\n");
  }

  printf("[");

  for (int i = 0; i < size - 1; i++) {
    printf("%f, ", arr[i]);
  }

  printf("%f]\n", arr[size - 1]);
}

float *init(int size, int rank) {
  float *arr = (float *)malloc(size * sizeof(float));

  for (int i = 0; i < size; i++) {
    arr[i] = 0.0;
  }

  if (rank == 0) {
    arr[0] = 100.0;
  }

  return arr;
}

void swap(float **t1, float **t2) {
  float *TEMP = *t1;
  *t1 = *t2;
  *t2 = TEMP;
}
