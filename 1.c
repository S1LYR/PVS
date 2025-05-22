#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void sequential_sum(int *array, int size) {
    long long sum = 0;
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    printf("Sequential sum: %lld\n", sum);
}

void parallel_sum(int *array, int size, int threads) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum) num_threads(threads)
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    printf("Parallel sum (%d threads): %lld\n", threads, sum);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <array_size> <num_threads>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    int threads = atoi(argv[2]);

    int *array = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 100;
    }

    double start = omp_get_wtime();
    if (threads == 1) {
        sequential_sum(array, size);
    } else {
        parallel_sum(array, size, threads);
    }
    double end = omp_get_wtime();

    printf("Time: %.6f sec\n", end - start);
    free(array);
    return 0;
}
