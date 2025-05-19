#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000  // 100 миллионов элементов

// Последовательное вычисление суммы
long long sequential_sum(int *array) {
    long long sum = 0;
    for (int i = 0; i < N; i++) {
        sum += array[i];
    }
    return sum;
}

// Параллельное вычисление суммы
long long parallel_sum(int *array, int threads) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum) num_threads(threads)
    for (int i = 0; i < N; i++) {
        sum += array[i];
    }
    return sum;
}

void benchmark() {
    int *array = malloc(N * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    // Инициализация массива
    for (int i = 0; i < N; i++) {
        array[i] = (i % 100) + 1;  // Значения 1..100
    }

    // Эталонное значение (последовательно)
    double seq_start = omp_get_wtime();
    long long seq_result = sequential_sum(array);
    double seq_time = omp_get_wtime() - seq_start;

    printf("Array size: %d elements | Expected sum: %lld\n\n", N, seq_result);
    printf("Mode       | Threads | Time (sec) | Speedup | Correct\n");
    printf("-----------+---------+------------+---------+--------\n");
    printf("Sequential |      1  | %9.5f  |  1.00x  |   yes\n", seq_time);

    // Тестируем параллельные версии
    int thread_counts[] = {2, 4, 8, 16};
    for (int i = 0; i < 4; i++) {
        int threads = thread_counts[i];
        double par_start = omp_get_wtime();
        long long par_result = parallel_sum(array, threads);
        double par_time = omp_get_wtime() - par_start;

        printf("Parallel   |    %2d   | %9.5f  | %6.2fx  |   %s\n",
               threads, par_time, seq_time / par_time,
               (par_result == seq_result) ? "yes" : "NO!");
    }

    free(array);
}

int main() {
    printf("Available CPU cores: %d\n", omp_get_num_procs());
    benchmark();
    return 0;
}