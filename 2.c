#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int *array, int low, int high) {
    int pivot = array[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (array[j] < pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[high]);
    return i + 1;
}

void quicksort_sequential(int *array, int low, int high) {
    if (low < high) {
        int pi = partition(array, low, high);
        quicksort_sequential(array, low, pi - 1);
        quicksort_sequential(array, pi + 1, high);
    }
}

void quicksort_parallel(int *array, int low, int high, int threads) {
    if (low < high) {
        int pi = partition(array, low, high);
        #pragma omp task shared(array) if (threads > 1)
        quicksort_parallel(array, low, pi - 1, threads / 2);
        #pragma omp task shared(array) if (threads > 1)
        quicksort_parallel(array, pi + 1, high, threads / 2);
    }
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
        quicksort_sequential(array, 0, size - 1);
    } else {
        #pragma omp parallel num_threads(threads)
        {
            #pragma omp single
            quicksort_parallel(array, 0, size - 1, threads);
        }
    }
    double end = omp_get_wtime();

    printf("Time: %.6f sec\n", end - start);
    free(array);
    return 0;
}
