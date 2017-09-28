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
      printf("%d ", m[i][j]);
    printf("\n");
  }
}

int main() {
  int i;
  int j;
  int k;

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      A[i][j] = B[i][j] = i + j;

  for (i = 0; i < N; i++)
    for (k = 0; k < N; k++)
      for (j = 0; j < N; j++)
        C[i][j] = A[i][k] + B[k][j];

  print_matrix(C);

  return 0;
}
