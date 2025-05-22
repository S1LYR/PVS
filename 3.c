#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void operations(int *a, int *b, int size, int threads) {
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < size; i++) {
        int sum = a[i] + b[i];
        int diff = a[i] - b[i];
        int prod = a[i] * b[i];
        float div = (b[i] != 0) ? (float)a[i] / b[i] : 0;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <array_size> <num_threads>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    int threads = atoi(argv[2]);

    int *a = malloc(size * sizeof(int));
    int *b = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        a[i] = rand() % 100 + 1;
        b[i] = rand() % 100 + 1;
    }

    double start = omp_get_wtime();
    operations(a, b, size, threads);
    double end = omp_get_wtime();

    printf("Time: %.6f sec\n", end - start);
    free(a);
    free(b);
    return 0;
}
