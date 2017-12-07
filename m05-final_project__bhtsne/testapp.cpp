#include <cstdio>
#include <omp.h>

int fib(int n) {
    int x, y;
    printf("Fib(%d) at thread %d\n", n, omp_get_thread_num());

    if (n == 0) {
        return 0;
    }

    if (n == 1) {
        return 1;
    }

#pragma omp parallel
#pragma omp single nowait
    {
#pragma omp task shared(x)
//        printf("Thread: %d\n", omp_get_thread_num());
        x = fib(n - 1);

#pragma omp task shared(y)
//        printf("Thread: %d\n", omp_get_thread_num());
        y = fib(n - 2);
#pragma omp taskwait
    }

    return x + y;
}

int main() {
    printf("Result: %d", fib(6));
    return 0;
}
