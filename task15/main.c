

#include <stdio.h>
#include <stdlib.h>

float atualizar_ponto(float *arr, int i) {
  return arr[i] + (0.4 * (arr[i - 1] - (2 * arr[i]) + arr[i + 1]));
}

void apply_one_step(float *t1, float *t2, int size) {
  for (int i = 1; i < size - 1; i++) {
    t2[i] = atualizar_ponto(t1, i);
  }
  t2[0] = t2[1];
  t2[size - 1] = t2[size - 2];
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

void init(float *arr, int size) {
  for (int i = 0; i < size; i++) {
    if (i == 0) {
      arr[i] = (float)size;
    } else {
      arr[i] = 0;
    }
  }
}

int main() {
  int size = 100;
  int steps = 500;
  float *t1 = (float *)malloc(size * sizeof(float));
  float *t2 = (float *)malloc(size * sizeof(float));

  init(t1, size);
  init(t2, size);

  print_arr(t1, size);
  for (int i = 0; i < steps; i++) {
    apply_one_step(t1, t2, size);

    float *TEMP = NULL;
    TEMP = t1;
    t1 = t2;
    t2 = TEMP;
  }
  printf("Array depois de %d steps: \n", steps);
  print_arr(t1, size);
}
