#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 200000000
#define TOL 0.0000001
//
//  This is a simple program to add two vectors
//  and verify the results.
//
//  History: Written by Tim Mattson, November 2017
//
int main() {

  double *a = (double *)malloc(sizeof(double) * N);
  double *b = (double *)malloc(sizeof(double) * N);
  double *c = (double *)malloc(sizeof(double) * N);
  double *res = (double *)malloc(sizeof(double) * N);
  int err = 0;

  double init_time, compute_time, test_time;
  init_time = -omp_get_wtime();

// fill the arrays
#pragma omp parallel for
  for (int i = 0; i < N; i++) {
    a[i] = (double)i;
    b[i] = 2.0 * (double)i;
    c[i] = 0.0;
    res[i] = i + 2 * i;
  }

  init_time += omp_get_wtime();
  compute_time = -omp_get_wtime();

#pragma omp target teams distribute parallel for map(to : a[0 : N], b[0 : N])  \
    map(from : c[0 : N])
  for (int i = 0; i < N; i++) {
    c[i] = a[i] + b[i];
  }

  compute_time += omp_get_wtime();
  test_time = -omp_get_wtime();

// test results
#pragma omp parallel for reduction(+ : err)
  for (int i = 0; i < N; i++) {
    double val = c[i] - res[i];
    val = val * val;
    if (val > TOL)
      err++;
  }

  test_time += omp_get_wtime();

  printf(" vectors added with %d errors\n", err);

  free(a);
  free(b);
  free(c);
  free(res);

  printf("Init time:    %.3fs\n", init_time);
  printf("Compute time: %.3fs\n", compute_time);
  printf("Test time:    %.3fs\n", test_time);
  printf("Total time:   %.3fs\n", init_time + compute_time + test_time);

  return 0;
}
