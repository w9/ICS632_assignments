#include <omp.h>
#include <stdio.h>

#ifndef N
#define N 10
#endif

int A[N][N];
int B[N][N];
int C[N][N];

void print_matrix(int m[N][N]) {
  int i;
  int j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++)
      fprintf(stderr, "%d ", m[i][j]);
    fprintf(stderr, "\n");
  }
}

int trace(int m[N][N]) {
  int i;
  int acc = 0;

  for (i = 0; i < N; i++) {
    acc += m[i][i];
  }

  return acc;
}

int main() {
  int i;
  int j;
  int k;

  // Initialize A and B.
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      A[i][j] = B[i][j] = i + j;
      C[i][j] = 0;
    }

#pragma omp parallel shared(A, B, C) private(i, j, k) num_threads(N_THREADS)
  {

#ifdef _OPENMP
    int np = omp_get_num_threads();
    int iam = omp_get_thread_num();
    fprintf(stderr, "(%d/%d) ", iam, np);
#endif

#pragma omp barrier

#if PAR_LOOP == 0
#pragma omp for
#endif
    for (i = 0; i < N; i++) {
#if PAR_LOOP == 1
#pragma omp for
#endif
      for (k = 0; k < N; k++) {
#if PAR_LOOP == 2
#pragma omp for
#endif
        for (j = 0; j < N; j++) {
#if PAR_LOOP == 1
#pragma omp atomic
#endif
          C[i][j] += A[i][k] * B[k][j];
        }
      }
    }
  }

  /* print_matrix(C); */
  printf("%d", trace(C));

  return 0;
}
