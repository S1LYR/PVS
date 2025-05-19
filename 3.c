#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define N 100000000  // 100 миллионов элементов
#define RANGE 100    // Диапазон значений: 1..100 (чтобы избежать деления на 0)

// Последовательные операции
void sequential_operations(double *a, double *b, double *results) {
    for (int i = 0; i < N; i++) {
        results[0*N + i] = a[i] + b[i];  // Сложение
        results[1*N + i] = a[i] - b[i];  // Вычитание
        results[2*N + i] = a[i] * b[i];  // Умножение
        results[3*N + i] = a[i] / b[i];  // Деление
    }
}

// Параллельные операции
void parallel_operations(double *a, double *b, double *results, int threads) {
    #pragma omp parallel for num_threads(threads)
    for (int i = 0; i < N; i++) {
        results[0*N + i] = a[i] + b[i];
        results[1*N + i] = a[i] - b[i];
        results[2*N + i] = a[i] * b[i];
        results[3*N + i] = a[i] / b[i];
    }
}

// Проверка корректности результатов
int verify_results(double *seq_results, double *par_results) {
    for (int i = 0; i < 4*N; i++) {
        if (fabs(seq_results[i] - par_results[i]) > 1e-9) {
            return 0;
        }
    }
    return 1;
}

void benchmark() {
    double *a = malloc(N * sizeof(double));
    double *b = malloc(N * sizeof(double));
    double *seq_results = malloc(4 * N * sizeof(double));
    double *par_results = malloc(4 * N * sizeof(double));
    
    if (!a || !b || !seq_results || !par_results) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    // Инициализация массивов
    for (int i = 0; i < N; i++) {
        a[i] = 1.0 + (i % RANGE);  // Значения 1.0..100.0
        b[i] = 1.0 + ((i+1) % RANGE); // Сдвиг на 1 для разнообразия
    }

    // Последовательные операции
    double seq_start = omp_get_wtime();
    sequential_operations(a, b, seq_results);
    double seq_time = omp_get_wtime() - seq_start;

    printf("Array size: %d elements | Operations: + - * /\n\n", N);
    printf("Mode       | Threads | Time (sec) | Speedup | Correct\n");
    printf("-----------+---------+------------+---------+--------\n");
    printf("Sequential |      1  | %9.5f  |  1.00x  |   yes\n", seq_time);

    // Параллельные операции
    int thread_counts[] = {2, 4, 8, 16};
    for (int i = 0; i < 4; i++) {
        int threads = thread_counts[i];
        
        double par_start = omp_get_wtime();
        parallel_operations(a, b, par_results, threads);
        double par_time = omp_get_wtime() - par_start;
        
        int correct = verify_results(seq_results, par_results);
        printf("Parallel   |    %2d   | %9.5f  | %6.2fx  |   %s\n",
               threads, par_time, seq_time / par_time,
               correct ? "yes" : "NO!");
    }

    free(a); free(b);
    free(seq_results); free(par_results);
}

int main() {
    printf("Available CPU cores: %d\n", omp_get_num_procs());
    benchmark();
    return 0;
}