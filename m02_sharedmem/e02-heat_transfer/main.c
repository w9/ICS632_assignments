/* #include "/usr/lib/gcc/x86_64-pc-linux-gnu/7.2.0/include/omp.h" */
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef N
#define N 1
#endif

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

#ifndef NUM_ITERS
#define NUM_ITERS 1
#endif

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1
#endif

int A[N + 2][N + 2];

void print_diagram(char c[], int n, int total) {
  int i;
  for (i = 0; i < total; i++) {
    if (i == n) {
      fprintf(stderr, "%s", c);
    } else {
      fprintf(stderr, ".");
    }
  }
  fprintf(stderr, " %d/%d: ", n, total);
}

int main() {
  int iter, i, j, k;

  // Fill in the array
  for (i = 0; i < N + 2; i++) {
    for (j = 0; j < N + 2; j++) {
      A[i][j] = i + j;
    }
  }

#if ALGO == 0
  // Old serial loop
  for (iter = 0; iter < NUM_ITERS; iter++) {
    for (i = 1; i < N + 1; i++) {
      for (j = 1; j < N + 1; j++) {
        A[i][j] =
            (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
      }
    }
  }
#endif



#if ALGO == 1
  omp_set_num_threads(NUM_THREADS);

#pragma omp parallel private(iter, i, j, k)
  {
#if DEBUG >= 1
#pragma omp critical
    {
      int np = omp_get_num_threads();
      int iam = omp_get_thread_num();
      print_diagram("i", iam, np);
      fprintf(stderr, "initialized\n");
    }
#endif

    for (iter = 0; iter < NUM_ITERS; iter++) {
      for (k = 1; k < N; k++) {
#pragma omp for
        for (j = 1; j < k + 1; j++) {
          i = k - j + 1;
          A[i][j] =
            (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
        }
      }
      for (k = N; k < 2 * N; k++) {
#pragma omp for
        for (j = k - N + 1; j < N + 1; j++) {
          i = k - j + 1;
          A[i][j] =
            (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
        }
      }
    }
  }
#endif



#if ALGO == 2
  omp_set_num_threads(NUM_THREADS);

#pragma omp parallel private(iter, i, j, k)
  {
#if DEBUG >= 1
    int np = omp_get_num_threads();
    int iam = omp_get_thread_num();
#pragma omp critical
    {
      print_diagram("i", iam, np);
      fprintf(stderr, "initialized\n");
    }
#endif

    int bi, bj;
    for (iter = 0; iter < NUM_ITERS; iter++) {
      for (bi = 0; bi < N; bi += BLOCK_SIZE) {
        for (bj = 0; bj < N; bj += BLOCK_SIZE) {
          int ni = (BLOCK_SIZE <= N - bi) ? BLOCK_SIZE : N - bi;
          int nj = (BLOCK_SIZE <= N - bj) ? BLOCK_SIZE : N - bj;

#if DEBUG >= 2
#pragma omp critical
          {
            print_diagram("b", iam, np);
            fprintf(stderr, "calculate block bi=%d bj=%d ni=%d nj=%d\n", bi, bj, ni, nj);
          }
#endif

          if (ni == nj) {
            for (k = bi + bj + 1; k < bi + bj + ni; k++) {
#pragma omp for
              for (j = bj + 1; j < - bi + k + 1; j++) {
                i = k - j + 1;
#if DEBUG >= 2
#pragma omp critical
                {
                  print_diagram("b", iam, np);
                  fprintf(stderr, "calculate cell i=%d j=%d k=%d\n", i, j, k);
                }
#endif

                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }

            for (k = bi + bj + ni; k < bi + bj + 2 * ni; k++) {
#pragma omp for
              for (j = - bi + k - ni + 1; j < bj + ni + 1; j++) {
                i = k - j + 1;

#if DEBUG >= 2
#pragma omp critical
                {
                  print_diagram("c", iam, np);
                  fprintf(stderr, "calculate cell i=%d j=%d k=%d\n", i, j, k);
                }
#endif
                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }

          } else if (ni < nj) {
            for (k = bi + bj + 1; k < bi + bj + ni; k++) {
#pragma omp for
              for (j = bj + 1; j < - bi + k + 1; j++) {
                i = k - j + 1;
                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }

            for (k = bi + bj + ni; k < bi + bj + nj; k++) {
#pragma omp for
              for (j = - bi + k - ni + 1; j < - bi + k + 1; j++) {
                i = k - j + 1;
                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }

            for (k = bi + bj + nj; k < bi + bj + ni + nj; k++) {
#pragma omp for
              for (j = - bi + k - ni + 1; j < bj + nj + 1; j++) {
                i = k - j + 1;
                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }
          } else {
            for (k = bi + bj + 1; k < bi + bj + nj; k++) {
#pragma omp for
              for (j = bj + 1; j < - bi + k + 1; j++) {
                i = k - j + 1;
                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }

            for (k = bi + bj + nj; k < bi + bj + ni; k++) {
#pragma omp for
              for (j = bj + 1; j < bj + nj + 1; j++) {
                i = k - j + 1;
                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }

            for (k = bi + bj + ni; k < bi + bj + ni + nj; k++) {
#pragma omp for
              for (j = - bi + k - ni + 1; j < bj + nj + 1; j++) {
                i = k - j + 1;
                A[i][j] =
                  (3 * A[i - 1][j] + A[i + 1][j] + 3 * A[i][j - 1] + A[i][j + 1]) / 4;
              }
            }
          }

#if DEBUG >= 2
#pragma omp critical
          {
            print_diagram("b", iam, np);
            fprintf(stderr, "end of block.\n");
          }
#endif


        }
      }
    }
  }
#endif




  int hash = 0;
  for (i = 0; i < N + 2; i++) {
    for (j = 0; j < N + 2; j++) {
      hash = ((hash << 5) + hash) + A[i][j]; // djb2 hashing
#if DEBUG >= 3
      fprintf(stderr, "%d ", A[i][j]);
#endif
    }
#if DEBUG >= 3
    fprintf(stderr, "\n");
#endif
  }
  printf("%d", hash);

  exit(0);
}
