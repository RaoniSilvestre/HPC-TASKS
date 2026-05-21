#include <omp.h>
#include <stdio.h>

int buffer[1024];
int insert = 0;
int consume = 0;
int count = 0;

void producer(int arg) {
#pragma omp critical
  {
    printf("Inserindo %d na pos %d\n", arg, insert);
    buffer[insert] = arg;
    insert = (insert + 1) % 1024;
    // Adiciona mais um na contagem, indicando que tem algo pra ser consumido.
    count++;
  }
}

int consumer() {
#pragma omp critical
  int v = -1;
  {
    // Só consome se tiver alguem para ser consumido.
    if (count > 0) {
      v = buffer[consume];
      consume = (consume + 1) % 1024;
      count--;
      printf("Buscando dado %d na pos %d\n", v, consume);
    }
  }
  // Retorna -1 se não tinha como ler
  return v;
}

int main() {
#pragma omp parallel
  {
#pragma omp master
    {
      for (int i = 0; i < 10; i++) {
        producer(3 * i + 15 * 4 / 3);
      }
    }

#pragma omp single
    {
      for (int i = 0; i < 10; i++) {
        int val = -1;
        while (val == -1) {
          val = consumer();
        }
      }
    }
  }
  return 0;
}
