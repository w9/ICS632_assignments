#include <stdio.h>

#define IJ 0
#define JI 1
#define TILED 2

#ifndef N
  #define N 10
#endif

double A[N][N];
double B[N][N];

int main() {
  int i, j;

  // DEBUG
  /* for (i = 0; i < N; i++) */
  /*   for (j = 0; j < N; j++) { */
  /*     A[i][j] = 0; */
  /*     B[i][j] = i * N + j; */
  /*   } */
  // END_DEBUG

#if ALGO == IJ
  // DEBUG
  /* printf("ALGO == IJ\n"); */
  // END_DEBUG

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      A[i][j] += B[j][i];

#elif ALGO == JI
  // DEBUG
  /* printf("ALGO == JI\n"); */
  // END_DEBUG

  for (j = 0; j < N; j++)
    for (i = 0; i < N; i++)
      A[i][j] += B[j][i];

#elif ALGO == TILED

  // DEBUG
  /* printf("ALGO == TILED\n"); */
  // END_DEBUG

#ifndef BS
  #define BS 4
#endif

  int ti, tj;

  for (ti = 0; ti < N; ti += BS)
    for (tj = 0; tj < N; tj += BS)
      for (i = ti; i < ti + BS && i < N; i++)
        for (j = tj; j < tj + BS && j < N; j++)
          A[i][j] += B[j][i];

#endif

  // DEBUG
  /* for (i = 0; i < N; i++) { */
  /*   for (j = 0; j < N; j++) */
  /*     printf("%f, ", A[i][j]); */
  /*   printf("\n"); */
  /* } */
  // END_DEBUG

  return 0;
}
