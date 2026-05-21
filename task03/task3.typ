#set page(
  paper: "a4",
  margin: (x: 2cm, y: 2.5cm),
  header: align(right, text(8pt, gray)[Aproximação matemática de PI]),
)


#show raw.where(block: true): it => {
  block(
    width: 100%,
    fill: luma(248),
    inset: 10pt,
    radius: 4pt,
    stroke: 0.5pt + luma(200),
    breakable: true, 
    it
  )
}

#set raw(lang: "c")

= Aproximação matemática de PI 

```c
#include "stdlib.h"
#include <stdio.h>
#include <time.h>

long long iters = 10000000000;

int main(int argc, char *argv[]) {
  double pi = 0;
  double sign = 1;

  struct timespec start, end;
  double elapsed;

  clock_gettime(CLOCK_MONOTONIC, &start);

  // Aproximação usando a sequencia de liebnitzz
  for (size_t i = 1; i < iters; i += 2) {
    pi += (1 / (float)i) * sign;
    sign = -sign;
  }

  pi = 4 * pi;

  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  printf("Iters: %lld ", iters);
  printf("Elapsed wall time:  %f seconds\n", elapsed);
  printf("Aprox: %f\n", pi);

  return 0;
}

//     ~/programming/hpc   at 18:18:07 
// ❯ ./a.out
// Iters: 1000000 Elapsed wall time:  0.002510 seconds
// Aprox: 3.141591
//
//     ~/programming/hpc   at 18:18:08 
// ❯ clang task3.c
//
//     ~/programming/hpc   at 18:18:14 
// ❯ ./a.out
// Iters: 100000000 Elapsed wall time:  0.250464 seconds
// Aprox: 3.141593
//
//     ~/programming/hpc   at 18:18:16 
// ❯ clang task3.c
//
//     ~/programming/hpc   at 18:18:22 
// ❯ ./a.out
// Iters: 10000000000 Elapsed wall time:  24.917377 seconds
// Aprox: 3.141593
```
